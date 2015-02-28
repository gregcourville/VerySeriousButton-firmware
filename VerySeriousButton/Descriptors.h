#pragma once

#include <avr/pgmspace.h>
#include "lufa_includes.h"

#define HID_IN_EPNUM 1
#define HID_EPSIZE 64

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t HID_HIDData;
	USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
	} USB_Descriptor_Configuration_t;

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
	 const uint8_t wIndex,
	 const void** const DescriptorAddress)
	  ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);
