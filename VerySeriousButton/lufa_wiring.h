#pragma once

#include "Descriptors.h"
#include "VerySeriousButton.h"
#include "vsb_config.h"
#include "lufa_includes.h"

#define REPORT_BUFFER_SIZE MAX(MAX(sizeof(USB_KeyboardReport_Data_t), sizeof(USB_JoystickReport_Data_t)),VSB_FEATREP_SIZE)

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

extern USB_ClassInfo_HID_Device_t Device_HID_Interface;
extern uint8_t PrevHIDReportBuffer[REPORT_BUFFER_SIZE];