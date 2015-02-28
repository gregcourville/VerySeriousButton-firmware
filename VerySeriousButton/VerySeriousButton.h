#pragma once

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#include "Descriptors.h"
#include "vsb_config.h"
#include "lufa_includes.h"

typedef struct {
	uint8_t Button;
	} USB_JoystickReport_Data_t; //TODO: Just use a plain uint8_t for this instead of a struct?

typedef struct {
	unsigned mode :2;
	uint8_t singleKeyCodes[VSB_SINGLEKEY_NKEYS];
	uint8_t singleKeyMods;
	uint8_t keyseqLen;
	} vsb_config_t;

void EVENT_USB_Device_StartOfFrame(void);
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
	 uint8_t* const reportId,
	 const uint8_t reportType,
	 void* reportData,
	 uint16_t* const reportsize);
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
	 const uint8_t reportId,
	 const uint8_t reportType,
	 const void* reportData,
	 const uint16_t reportSize);
