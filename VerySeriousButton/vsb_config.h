#pragma once

#define VSB_SINGLEKEY_NKEYS 6
#define VSB_KEYSEQ_NKEYS 2
#define VSB_KEYSEQ_PGSIZE (VSB_KEYSEQ_NKEYS+1)
#define VSB_KEYSEQ_NPAGES 150

#define VSB_REPORTID_KBD 1
#define VSB_REPORTID_JOY 2
#define VSB_REPORTID_VSB 3

#define VSB_CMDDATA_LEN 32
#define VSB_FEATREP_SIZE (VSB_CMDDATA_LEN+2)

#define DEBOUNCE_TIME 50
#define REBOOT_DELAY 1000

#define VSB_CMD_NONE 0
#define VSB_CMD_GETDEVINFO 1
#define VSB_CMD_GETCFG 2
#define VSB_CMD_SETCFG 3
#define VSB_CMD_SAVECFG 4
#define VSB_CMD_LOADCFG 5
#define VSB_CMD_WIPECFG 6
#define VSB_CMD_READPAGE 7
#define VSB_CMD_WRITEPAGE 8
#define VSB_CMD_GETSERIAL 0x11
#define VSB_CMD_EEPREAD 0xF1
#define VSB_CMD_EEPWRITE 0xF2
#define VSB_CMD_RESET 0xF3
#define VSB_CMD_DFU 0xF4
#define VSB_CMD_FUCKYOU 0xF0

#define VSB_RESP_NULL 0
#define VSB_RESP_OK 1
#define VSB_RESP_ERR 0x10
#define VSB_RESP_BADCMD 0x11
#define VSB_RESP_BADCS 0x12
#define VSB_RESP_BADMEM 0x14
#define VSB_RESP_BADIDX 0x18
#define VSB_RESP_BUSY 0x80

#define VSB_MODE_NONE 0
#define VSB_MODE_JOYSTICK 1
#define VSB_MODE_SINGLEKEY 2
#define VSB_MODE_KEYSEQ 3