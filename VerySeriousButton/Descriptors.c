#include "Descriptors.h"
#include "vsb_config.h"
#include "vsb_serial.h"
#include "VerySeriousButton.h"

const USB_Descriptor_HIDReport_Datatype_t PROGMEM HIDReport[] = {
	HID_RI_USAGE_PAGE(8, 0x01),
	HID_RI_USAGE(8, 0x05),
	HID_RI_COLLECTION(8, 0x01),
	 HID_RI_REPORT_ID(8, VSB_REPORTID_JOY),
	 HID_RI_USAGE_PAGE(8, 0x09),
	 HID_RI_USAGE_MINIMUM(8, 0x01),
	 HID_RI_USAGE_MAXIMUM(8, 0x01),
	 HID_RI_LOGICAL_MINIMUM(8, 0x00),
	 HID_RI_LOGICAL_MAXIMUM(8, 0x01),
	 HID_RI_REPORT_SIZE(8, 0x01),
	 HID_RI_REPORT_COUNT(8, 0x01),
	 HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
	 HID_RI_REPORT_COUNT(8, 0x07),
	 HID_RI_REPORT_SIZE(8, 0x01),
	 HID_RI_INPUT(8, HID_IOF_CONSTANT),
	HID_RI_END_COLLECTION(0),
	HID_RI_USAGE_PAGE(8, 0x01),
	 HID_RI_USAGE(8, 0x06),
	 HID_RI_COLLECTION(8, 0x01),
	 HID_RI_REPORT_ID(8, VSB_REPORTID_KBD),
	 HID_RI_USAGE_PAGE(8, 0x07),
	 HID_RI_USAGE_MINIMUM(8, 0xE0),
	 HID_RI_USAGE_MAXIMUM(8, 0xE7),
	 HID_RI_LOGICAL_MINIMUM(8, 0x00),
	 HID_RI_LOGICAL_MAXIMUM(8, 0x01),
	 HID_RI_REPORT_SIZE(8, 0x01),
	 HID_RI_REPORT_COUNT(8, 0x08),
	 HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
	 HID_RI_REPORT_COUNT(8, 0x01),
	 HID_RI_REPORT_SIZE(8, 0x08),
	 HID_RI_INPUT(8, HID_IOF_CONSTANT),
	 HID_RI_USAGE_PAGE(8, 0x08), 
	 HID_RI_USAGE_MINIMUM(8, 0x01), 
	 HID_RI_USAGE_MAXIMUM(8, 0x05), 
	 HID_RI_REPORT_COUNT(8, 0x05),
	 HID_RI_REPORT_SIZE(8, 0x01),
	 HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
	 HID_RI_REPORT_COUNT(8, 0x01),
	 HID_RI_REPORT_SIZE(8, 0x03),
	 HID_RI_OUTPUT(8, HID_IOF_CONSTANT),
	 HID_RI_LOGICAL_MINIMUM(8, 0x00),
	 HID_RI_LOGICAL_MAXIMUM(8, 0x65),
	 HID_RI_USAGE_PAGE(8, 0x07),
	 HID_RI_USAGE_MINIMUM(8, 0x00),
	 HID_RI_USAGE_MAXIMUM(8, 0x65),
	 HID_RI_REPORT_COUNT(8, 0x06),
	 HID_RI_REPORT_SIZE(8, 0x08),
	 HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
	HID_RI_END_COLLECTION(0),
	HID_RI_USAGE_PAGE(16, 0xFF01),
	HID_RI_USAGE(8, 0x01),
	HID_RI_COLLECTION(8, 0x01),
	 HID_RI_REPORT_ID(8, VSB_REPORTID_VSB),
	 HID_RI_USAGE(8, 0x02),
	 HID_RI_LOGICAL_MINIMUM(8, 0x00),
	 HID_RI_LOGICAL_MAXIMUM(8, 0xFF), 
	 HID_RI_REPORT_SIZE(8, 8), 
	 HID_RI_REPORT_COUNT(8, VSB_FEATREP_SIZE), 
	 HID_RI_FEATURE(16, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NO_PREFERRED_STATE | HID_IOF_VOLATILE | HID_IOF_BUFFERED_BYTES),
	HID_RI_END_COLLECTION(0)
	};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
	.Header = {
		.Size = sizeof(USB_Descriptor_Device_t),
		.Type = DTYPE_Device
		},
	.USBSpecification       = VERSION_BCD(01.10),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,
	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
	.VendorID               = 0x16D0,
	.ProductID              = 0x09D2,
	.ReleaseNumber          = VERSION_BCD(01.00),
	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = 0x03,
	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
	};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
	.Config = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Configuration_Header_t),
			.Type = DTYPE_Configuration
			},
		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces = 1,
		.ConfigurationNumber = 1,
		.ConfigurationStrIndex = NO_DESCRIPTOR,
		.ConfigAttributes = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
		.MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
		},
	.HID_Interface = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Interface_t),
			.Type = DTYPE_Interface
			},
		.InterfaceNumber        = 0x00,
		.AlternateSetting       = 0x00,
		.TotalEndpoints         = 1,
		.Class                  = HID_CSCP_HIDClass,
		.SubClass               = HID_CSCP_NonBootSubclass,
		.Protocol               = HID_CSCP_NonBootProtocol,
		.InterfaceStrIndex      = NO_DESCRIPTOR
		},
	.HID_HIDData = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID
			},
		.HIDSpec = VERSION_BCD(01.11),
		.CountryCode = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType = HID_DTYPE_Report,
		.HIDReportLength = sizeof(HIDReport)
		},
	.HID_ReportINEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
			},
		.EndpointAddress = (ENDPOINT_DESCRIPTOR_DIR_IN | HID_IN_EPNUM),
		.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize = HID_EPSIZE,
		.PollingIntervalMS = 0x01
		},
	};

const USB_Descriptor_String_t PROGMEM LanguageString = {
	.Header = {
		.Size = USB_STRING_LEN(1),
		.Type = DTYPE_String
		},
	.UnicodeString = {LANGUAGE_ID_ENG}
	};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
	.Header = {
		.Size = USB_STRING_LEN(8),
		.Type = DTYPE_String
		},
	.UnicodeString = L"GregLabs"
	};

const USB_Descriptor_String_t PROGMEM ProductString = {
	.Header = {
		.Size = USB_STRING_LEN(19),
		.Type = DTYPE_String
		},
	.UnicodeString = L"Very Serious Button"
	};

const USB_Descriptor_String_t PROGMEM SerialNumString = {
	.Header = {
		.Size = USB_STRING_LEN(VSB_SERIAL_NO_LEN),
		.Type = DTYPE_String
		},
	.UnicodeString = VSB_SERIAL_NO_STRL
	};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
	 const uint8_t wIndex, const void** const DescriptorAddress) {
	const uint8_t DescriptorType = (wValue >> 8);
	const uint8_t DescriptorNumber = (wValue & 0xFF);
	const void* Address = NULL;
	uint16_t Size = NO_DESCRIPTOR;
	switch (DescriptorType) {
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber) {
				case 0x00:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
					Address = &SerialNumString;
					Size    = pgm_read_byte(&SerialNumString.Header.Size);
					break;
				}
			break;
		case HID_DTYPE_HID:
			Address = &ConfigurationDescriptor.HID_HIDData;
			Size    = sizeof(USB_HID_Descriptor_HID_t);			
			break;
		case HID_DTYPE_Report:
			Address = &HIDReport;
			Size    = sizeof(HIDReport);
			break;
		}
	*DescriptorAddress = Address;
	return Size;
	}

