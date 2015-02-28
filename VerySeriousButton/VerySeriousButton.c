#include "VerySeriousButton.h"
#include "lufa_wiring.h"
#include "vsb_serial.h"
#include "vsb_config.h"
#include "board_config.h"
#include <stddef.h>

#define BTN_DOWN() (((BTN_ACTIVE_LOW) ? (BTN_PIN & BTN_BMASK) : !(BTN_PIN & BTN_BMASK)) ? 1 : 0)

#define EEP_LEN_NVCONFIG sizeof(vsb_config_t)
#define EEP_LEN_KEYSEQ (VSB_KEYSEQ_PAGE_SIZE * VSB_NUM_KEYSEQ_PAGES)
#define EEP_ADDR_NVCONFIG (uint8_t *)0
#define EEP_ADDR_CHKSUM (uint8_t *)(EEP_LEN_NVCONFIG)
#define EEP_ADDR_KEYSEQ (EEP_ADDR_CHKSUM+1)

bool loadNvConfig(vsb_config_t *cfg);
void loadFactoryConfig(vsb_config_t *cfg);
uint8_t getChecksum(const void *buf, size_t n);

volatile struct {
	uint8_t debounceTimer;
	uint8_t singlekeyIndex;
	uint8_t keyseqIndex;
	uint8_t cmdId;
	uint8_t cmdData[VSB_CMDDATA_LEN];
	uint8_t respId;
	uint8_t respData[VSB_CMDDATA_LEN];
	uint8_t slurIndex;
	uint16_t rebootTimer;
	unsigned slowCmdBusy :1;
	unsigned rebootTimerActive :1;
	unsigned keyseqInProgress :1;
	unsigned keyseqPhase :1;
	unsigned btnHeld :1;
	unsigned rebootToDfu :1;
	unsigned debounceTimerActive :1;
	unsigned lastBtnState :1;
	unsigned nvCfgLoadFailed :1;
	unsigned nextReport :2;
	} vsbStatus;

struct {
	uint8_t cmdId;
	uint8_t respId;
	uint8_t data[VSB_CMDDATA_LEN];
	} vsbCmdResponse;

static inline void initVsbStatus() {
	memset(&vsbStatus, 0, sizeof(vsbStatus));
	}

static inline void initBtn() {
	BTN_DDR &= ~BTN_BMASK;
	BTN_PORT |= BTN_BMASK;
	}

vsb_config_t vsbConfig;

bool loadNvConfig(vsb_config_t *cfg) {
	uint8_t buf[sizeof(vsb_config_t)];
	uint8_t nvcCs, calcCs;
	eeprom_read_block(buf, EEP_ADDR_NVCONFIG, EEP_LEN_NVCONFIG);
	nvcCs = eeprom_read_byte(EEP_ADDR_CHKSUM);
	calcCs = getChecksum(buf, sizeof(vsb_config_t));
	if(nvcCs == calcCs) {
		memcpy(cfg, buf, sizeof(vsb_config_t));
		return true;
		}
	return false;
	}

uint8_t getChecksum(const void *buf, size_t n) {
	uint8_t cs = 0;
	for(size_t i = 0; i<n; ++i) {
		cs += *((uint8_t *)buf + i);
		}
	return cs;
	}

bool saveNvConfig(const vsb_config_t *cfg) {
	uint8_t cs = getChecksum(cfg, sizeof(vsb_config_t));
	vsb_config_t buf;
	eeprom_update_block(cfg, EEP_ADDR_NVCONFIG, EEP_LEN_NVCONFIG);
	eeprom_update_byte(EEP_ADDR_CHKSUM, cs);
	eeprom_read_block(&buf, EEP_ADDR_NVCONFIG, EEP_LEN_NVCONFIG);
	if (memcmp(&buf, cfg, sizeof(vsb_config_t)) || (cs != eeprom_read_byte(EEP_ADDR_CHKSUM)))
		return false;
	return true;
	}

bool wipeNvConfig() {
	vsb_config_t factoryConfig;
	loadFactoryConfig(&factoryConfig);
	return saveNvConfig(&factoryConfig);
	}

inline void debounceTimerTick() {
	if(vsbStatus.debounceTimerActive) {
		if(++vsbStatus.debounceTimer > DEBOUNCE_TIME) {
			vsbStatus.debounceTimerActive = 0;
			}
		}
	}

void reset() {
	wdt_enable(WDTO_250MS);
	while(1) {}
	}

void resetToBootloader() {
	asm volatile ("ijmp" ::"z" (0x1800));
	}

inline void rebootTimerTick() {
	if(vsbStatus.rebootTimerActive) {
		if(++vsbStatus.rebootTimer > REBOOT_DELAY) {
			if(vsbStatus.rebootToDfu) {
				resetToBootloader();
				}
			else {
				reset();
				}
			}
		}
	}

bool getBtnStatus() {
	if(vsbStatus.debounceTimerActive)
		return vsbStatus.lastBtnState;
	uint8_t newBtnState = BTN_DOWN();
	if(vsbStatus.lastBtnState != newBtnState) {
		vsbStatus.debounceTimerActive = 1;
		vsbStatus.debounceTimer = 0;
		vsbStatus.lastBtnState = newBtnState;
		}
	return vsbStatus.lastBtnState;
	}

void readKeyseqPage(uint8_t *buf, uint8_t i) {
	eeprom_read_block(buf, EEP_ADDR_KEYSEQ + VSB_KEYSEQ_PGSIZE*i, VSB_KEYSEQ_PGSIZE);
	}

void loadFactoryConfig(vsb_config_t *cfg) {
	cfg->mode = VSB_MODE_JOYSTICK;
	memset(cfg->singleKeyCodes, 0, VSB_SINGLEKEY_NKEYS);
	cfg->singleKeyMods = 0;
	cfg->singleKeyCodes[0] = HID_KEYBOARD_SC_SPACE;
	cfg->keyseqLen = 0;
	}

void vsbInit() {
	initBtn();
	initVsbStatus();
	if(!loadNvConfig(&vsbConfig)) {
		vsbStatus.nvCfgLoadFailed = 1;
		loadFactoryConfig(&vsbConfig);
		}
	}

void hwInit(void) {
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);
	}

int main(void) {
	hwInit();
	vsbInit();
	USB_Init();
	sei();
	for (;;) {
		HID_Device_USBTask(&Device_HID_Interface);
		USB_USBTask();
		slowCmdTask();
		}
	}

void EVENT_USB_Device_StartOfFrame(void) {
	HID_Device_MillisecondElapsed(&Device_HID_Interface);
	debounceTimerTick();
	rebootTimerTick();
	}

bool CALLBACK_HID_Device_CreateHIDReport(
	  USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
	  uint8_t* const reportId, const uint8_t reportType, void* reportData,
	  uint16_t* const reportSize) {
	if((reportType == HID_REPORT_ITEM_Feature) && (*reportId == VSB_REPORTID_VSB)) {
		memset(reportData, 0, VSB_FEATREP_SIZE);
		*reportSize = VSB_FEATREP_SIZE;
		((uint8_t *)reportData)[0] = vsbStatus.cmdId;
		((uint8_t *)reportData)[1] = vsbStatus.respId;
		memcpy(((uint8_t *)reportData+2), vsbStatus.respData, sizeof(vsbStatus.respData));
		vsbStatus.cmdId = VSB_CMD_NONE;
		vsbStatus.respId = VSB_RESP_NULL;
		return true;
		}
	else if(reportType == HID_REPORT_ITEM_In) {
		bool btnStatus = getBtnStatus();
		if(!*reportId) {
			switch(vsbStatus.nextReport) {
				case VSB_REPORTID_JOY:
					*reportId = VSB_REPORTID_JOY;
					vsbStatus.nextReport = VSB_REPORTID_KBD;
					break;
				default:
					*reportId = VSB_REPORTID_KBD;
					vsbStatus.nextReport = VSB_REPORTID_JOY;
					break;
				}
			}
		if (*reportId == VSB_REPORTID_KBD) {
			USB_KeyboardReport_Data_t* kbdReport = (USB_KeyboardReport_Data_t*)reportData;
			*reportSize = sizeof(USB_KeyboardReport_Data_t);
			memset(kbdReport, 0, *reportSize);
			if(vsbConfig.mode == VSB_MODE_KEYSEQ) {
				if(!vsbStatus.keyseqInProgress) {
					if(vsbStatus.btnHeld && !btnStatus) {
						vsbStatus.btnHeld = 0;
						}
					if(btnStatus && !vsbStatus.btnHeld) {
						vsbStatus.btnHeld = 1;
						vsbStatus.keyseqInProgress = 1;
						vsbStatus.keyseqPhase = 0;
						vsbStatus.keyseqIndex = 0;
						}
					}
				if(vsbStatus.keyseqInProgress) {
					if(!vsbStatus.keyseqPhase) {
						uint8_t buf[VSB_KEYSEQ_PGSIZE];
						readKeyseqPage(buf, vsbStatus.keyseqIndex);
						kbdReport->Modifier = buf[0];
						memcpy(kbdReport->KeyCode, buf+1, VSB_KEYSEQ_PGSIZE-1);
						vsbStatus.keyseqPhase = 1;
						}
					else {
						vsbStatus.keyseqIndex++;
						vsbStatus.keyseqPhase = 0;
						if(vsbStatus.keyseqIndex >= vsbConfig.keyseqLen) {
							vsbStatus.keyseqInProgress = 0;
							}
						}
					}
				}
			else if(vsbConfig.mode == VSB_MODE_SINGLEKEY) {
				if (btnStatus) {
					kbdReport->Modifier = vsbConfig.singleKeyMods;
					memcpy(kbdReport->KeyCode, vsbConfig.singleKeyCodes, vsbStatus.singlekeyIndex++);
					if(vsbStatus.singlekeyIndex >= VSB_SINGLEKEY_NKEYS)
						vsbStatus.singlekeyIndex = VSB_SINGLEKEY_NKEYS - 1;
					}
				else {
					vsbStatus.singlekeyIndex = 0;
					}
				}
			return true;
			}
		else if (*reportId == VSB_REPORTID_JOY) {
			USB_JoystickReport_Data_t* joystickReport = (USB_JoystickReport_Data_t*)reportData;
			*reportSize = sizeof(USB_JoystickReport_Data_t);
			memset(joystickReport, 0, *reportSize);
			if(vsbConfig.mode == VSB_MODE_JOYSTICK) {
				joystickReport->Button = btnStatus;
				}
			return true;
			}
		}
	return false; //unnecessary
	}

inline uint16_t parseUint16Be(const uint8_t *buf) {
	return (uint16_t)buf[0] << 8 | buf[1];
	}

inline void packUint16Be(uint8_t *buf, uint16_t v) {
	buf[0] = (v & 0xFF00) >> 8;
	buf[1] = v & 0x00FF;
	}

inline void triggerReboot() {
	vsbConfig.mode = VSB_MODE_NONE;
	vsbStatus.rebootTimerActive = 1;
	vsbStatus.rebootTimer = 0;
	}

inline void triggerRebootToDfu() {
	vsbStatus.rebootToDfu = 1;
	triggerReboot();
	}

inline void scheduleSlowCmd(uint8_t cmdId, uint8_t *cmdData) {
	vsbStatus.respId = VSB_RESP_BUSY;
	vsbStatus.slowCmdBusy = 1;
	memcpy(vsbStatus.cmdData, cmdData, VSB_CMDDATA_LEN);
	}

void slowCmdTask() {
	if(!vsbStatus.slowCmdBusy)
		return;
	switch(vsbStatus.cmdId) {
		case VSB_CMD_EEPREAD:;
			uint16_t readAddr = parseUint16Be(&vsbStatus.cmdData[0]);
			packUint16Be(&vsbStatus.respData[0], readAddr);
			vsbStatus.respData[2] = eeprom_read_byte((uint8_t *)readAddr);
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_EEPWRITE:;
			uint16_t writeAddr = parseUint16Be(&vsbStatus.cmdData[0]);
			eeprom_update_byte((uint8_t *)writeAddr, vsbStatus.cmdData[2]);
			packUint16Be(&vsbStatus.respData[0], writeAddr);
			vsbStatus.respData[2] = vsbStatus.cmdData[2];
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_FUCKYOU:;
			switch(vsbStatus.slurIndex++) {
				case 0:
					sprintf((char *)vsbStatus.respData, "FUCK IT");
					break;
				case 1:
					sprintf((char *)vsbStatus.respData, "FUCK THIS");
					break;
				case 2:
					sprintf((char *)vsbStatus.respData, "FUCK YOU");
					break;
				default:
					sprintf((char *)vsbStatus.respData, "FUCK YOU %d TIMES OVER", (uint8_t)(vsbStatus.slurIndex-2));
				}
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_LOADCFG:;
			if(loadNvConfig(&vsbConfig)) {
				vsbStatus.respId = VSB_RESP_OK;
				}
			else {
				vsbStatus.respId = VSB_RESP_BADCS;
				}
			break;
		case VSB_CMD_SAVECFG:;
			if(saveNvConfig(&vsbConfig)) {
				vsbStatus.respId = VSB_RESP_OK;
				}
			else {
				vsbStatus.respId = VSB_RESP_BADMEM;
				}
			break;
		case VSB_CMD_WIPECFG:;
			wipeNvConfig();
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_READPAGE:;
			vsbStatus.respData[0] = vsbStatus.cmdData[0];
			if (vsbStatus.cmdData[0] >= VSB_KEYSEQ_NPAGES) {
				vsbStatus.respId = VSB_RESP_BADIDX;
				break;
				}
			readKeyseqPage(&vsbStatus.respData[1], vsbStatus.cmdData[0]);
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_WRITEPAGE:;
			vsbStatus.respData[0] = vsbStatus.cmdData[0];
			if (vsbStatus.cmdData[0] >= VSB_KEYSEQ_NPAGES) {
				vsbStatus.respId = VSB_RESP_BADIDX;
				break;
				}
			eeprom_write_block(&vsbStatus.cmdData[1], EEP_ADDR_KEYSEQ + (VSB_KEYSEQ_PGSIZE*vsbStatus.cmdData[0]), VSB_KEYSEQ_PGSIZE);
			vsbStatus.respId = VSB_RESP_OK;
			break;
		default:;
			break; //Fuck it
		}
	vsbStatus.slowCmdBusy = 0;
	}

inline void handleVsbCommand(uint8_t cmdId, uint8_t *cmdData) {
	switch(cmdId) {
		case VSB_CMD_NONE:;
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_GETSERIAL:;
			vsbStatus.respData[0] = VSB_SERIAL_NO_LEN;
			memcpy(&vsbStatus.respData[1], VSB_SERIAL_NO, VSB_SERIAL_NO_LEN);
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_RESET:;
			triggerReboot();
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_DFU:;
			triggerRebootToDfu();
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_SETCFG:;
			vsbConfig.mode = cmdData[0];
			vsbConfig.singleKeyMods = cmdData[1];
			memcpy(vsbConfig.singleKeyCodes, &cmdData[2], VSB_SINGLEKEY_NKEYS);
			vsbConfig.keyseqLen = cmdData[2 + sizeof(vsbConfig.singleKeyCodes)];
		case VSB_CMD_GETCFG:;
			vsbStatus.respData[0] = vsbConfig.mode;
			vsbStatus.respData[1] = vsbConfig.singleKeyMods;
			memcpy(&vsbStatus.respData[2], vsbConfig.singleKeyCodes, VSB_SINGLEKEY_NKEYS);
			vsbStatus.respData[2 + sizeof(vsbConfig.singleKeyCodes)] = vsbConfig.keyseqLen;
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_GETDEVINFO:;
			vsbStatus.respData[0] = VSB_SINGLEKEY_NKEYS;
			vsbStatus.respData[1] = VSB_KEYSEQ_NKEYS;
			vsbStatus.respData[2] = VSB_KEYSEQ_PGSIZE;
			vsbStatus.respData[3] = VSB_KEYSEQ_NPAGES;
			vsbStatus.respId = VSB_RESP_OK;
			break;
		case VSB_CMD_EEPREAD:;
		case VSB_CMD_EEPWRITE:;
		case VSB_CMD_FUCKYOU:;
		case VSB_CMD_LOADCFG:;
		case VSB_CMD_SAVECFG:;
		case VSB_CMD_WIPECFG:;
		case VSB_CMD_READPAGE:;
		case VSB_CMD_WRITEPAGE:;
			scheduleSlowCmd(cmdId, cmdData);
			break;
		default:;
			vsbStatus.respId = VSB_RESP_BADCMD;
		}
	}

void CALLBACK_HID_Device_ProcessHIDReport(
	 USB_ClassInfo_HID_Device_t* const interfaceInfo, const uint8_t reportId,
	 const uint8_t reportType, const void* reportData, const uint16_t reportsize) {
	if((reportType != HID_REPORT_ITEM_Feature) || (reportId != VSB_REPORTID_VSB))
		return; //Fuck it
	if(vsbStatus.slowCmdBusy)
		return; //Fuck that too
	vsbStatus.cmdId = ((uint8_t *)reportData)[0];
	vsbStatus.respId = VSB_RESP_NULL;
	memset(vsbStatus.respData, 0, sizeof(vsbStatus.respData));
	handleVsbCommand(vsbStatus.cmdId, (uint8_t *)reportData+2);
	}
