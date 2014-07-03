/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Silicon Labs, http://www.silabs.com

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"


/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t DeviceDescriptor = {
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(2, 0, 0),
	.Class                  = CDC_CSCP_CDCClass,
	.SubClass               = CDC_CSCP_NoSpecificSubclass,
	.Protocol               = CDC_CSCP_NoSpecificProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x10C4,
	.ProductID              = 0x89A1,
	.ReleaseNumber          = VERSION_BCD(0, 0, 1),

	.ManufacturerStrIndex   = STRING_ID_Manufacturer,
	.ProductStrIndex        = STRING_ID_Product,
	.SerialNumStrIndex      = USE_INTERNAL_SERIAL,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t ConfigurationDescriptor = {
	.Config =
	{
		.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces        = 2,

		.ConfigurationNumber    = 1,
		.ConfigurationStrIndex  = NO_DESCRIPTOR,

		.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

		.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
	},

	.CDC_CCI_Interface =
	{
		.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

		.InterfaceNumber        = INTERFACE_ID_CDC_CCI,
		.AlternateSetting       = 0,

		.TotalEndpoints         = 1,

		.Class                  = CDC_CSCP_CDCClass,
		.SubClass               = CDC_CSCP_ACMSubclass,
		.Protocol               = CDC_CSCP_ATCommandProtocol,

		.InterfaceStrIndex      = NO_DESCRIPTOR
	},

	.CDC_Functional_Header =
	{
		.Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
		.Subtype                = CDC_DSUBTYPE_CSInterface_Header,

		.CDCSpecification       = VERSION_BCD(1, 1, 0),
	},

	.CDC_Functional_ACM =
	{
		.Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
		.Subtype                = CDC_DSUBTYPE_CSInterface_ACM,

		.Capabilities           = 0x06,
	},

	.CDC_Functional_Union =
	{
		.Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
		.Subtype                = CDC_DSUBTYPE_CSInterface_Union,

		.MasterInterfaceNumber  = INTERFACE_ID_CDC_CCI,
		.SlaveInterfaceNumber   = INTERFACE_ID_CDC_DCI,
	},

	.CDC_NotificationEndpoint =
	{
		.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

		.EndpointAddress        = CDC_NOTIFICATION_EPADDR,
		.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = CDC_NOTIFICATION_EPSIZE,
		.PollingIntervalMS      = 0xFF
	},

	.CDC_DCI_Interface =
	{
		.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

		.InterfaceNumber        = INTERFACE_ID_CDC_DCI,
		.AlternateSetting       = 0,

		.TotalEndpoints         = 2,

		.Class                  = CDC_CSCP_CDCDataClass,
		.SubClass               = CDC_CSCP_NoDataSubclass,
		.Protocol               = CDC_CSCP_NoDataProtocol,

		.InterfaceStrIndex      = NO_DESCRIPTOR
	},

	.CDC_DataOutEndpoint =
	{
		.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

		.EndpointAddress        = CDC_RX_EPADDR,
		.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = CDC_TXRX_EPSIZE,
		.PollingIntervalMS      = 0x05
	},

	.CDC_DataInEndpoint =
	{
		.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

		.EndpointAddress        = CDC_TX_EPADDR,
		.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = CDC_TXRX_EPSIZE,
		.PollingIntervalMS      = 0x05
	}
};

#define STR0LEN 4
static uint8_t const String0Desc[STR0LEN] = {
	STR0LEN, DTYPE_String, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof("Silicon Laboratories Inc.")*2
static uint8_t const String1Desc[STR1LEN] = {
	STR1LEN, DTYPE_String,
	'S', 0,
	'i', 0,
	'l', 0,
	'i', 0,
	'c', 0,
	'o', 0,
	'n', 0,
	' ', 0,
	'L', 0,
	'a', 0,
	'b', 0,
	'o', 0,
	'r', 0,
	'a', 0,
	't', 0,
	'o', 0,
	'r', 0,
	'i', 0,
	'e', 0,
	's', 0,
	' ', 0,
	'I', 0,
	'n', 0,
	'c', 0,
	'.', 0
}; //end of String1Desc

#define STR2LEN sizeof("EFM32 CDC Device")*2
static uint8_t const String2Desc[STR2LEN] = {
	STR2LEN, DTYPE_String,
	'E', 0,
	'F', 0,
	'M', 0,
	'3', 0,
	'2', 0,
	' ', 0,
	'C', 0,
	'D', 0,
	'C', 0,
	' ', 0,
	'D', 0,
	'e', 0,
	'v', 0,
	'i', 0,
	'c', 0,
	'e', 0
}; //end of String2Desc

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void **const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void *Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

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
			Address = String0Desc;
			Size    = STR0LEN;
			break;
		case 0x01:
			Address = String1Desc;
			Size    = STR1LEN;
			break;
		case 0x02:
			Address = String2Desc;
			Size    = STR2LEN;
			break;
		}

		break;
	}

	*DescriptorAddress = Address;
	return Size;
}
