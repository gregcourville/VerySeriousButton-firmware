#include "lufa_wiring.h"

uint8_t PrevHIDReportBuffer[REPORT_BUFFER_SIZE];

USB_ClassInfo_HID_Device_t Device_HID_Interface =	{
	.Config = {
		.InterfaceNumber = 0,
		.ReportINEndpointNumber = HID_IN_EPNUM,
		.ReportINEndpointSize = HID_EPSIZE,
		.ReportINEndpointDoubleBank = false,
		.PrevReportINBuffer = PrevHIDReportBuffer,
		.PrevReportINBufferSize = sizeof(PrevHIDReportBuffer),
		},
	};

void EVENT_USB_Device_Connect(void) {
	}

void EVENT_USB_Device_Disconnect(void) {
	}

void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Device_HID_Interface);
	USB_Device_EnableSOFEvents();
	}

void EVENT_USB_Device_ControlRequest(void) {
	HID_Device_ProcessControlRequest(&Device_HID_Interface);
	}
