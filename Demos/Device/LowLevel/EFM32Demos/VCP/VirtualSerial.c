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
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include "VirtualSerial.h"
#include "em_usart.h"

extern void setupSWOForPrint(void);
#define SYSTICKHZ             1000

uint8_t EndpointDescriptors[] __attribute__((aligned(4))) = {
	/* Total Endpoints*/
	4,
	/* Control ENDPOINT*/
	7,
	DTYPE_Endpoint,
	ENDPOINT_CONTROLEP,
	EP_TYPE_CONTROL,
	FIXED_CONTROL_ENDPOINT_SIZE,
	0x00,
	0x00,
	/* Notify ENDPOINT*/
	7,
	DTYPE_Endpoint,
	CDC_NOTIFICATION_EPADDR,
	EP_TYPE_INTERRUPT,
	CDC_NOTIFICATION_EPSIZE,
	0x00,
	0x00,
	/* Bulk in ENDPOINT*/
	7,
	DTYPE_Endpoint,
	CDC_TX_EPADDR,
	EP_TYPE_BULK,
	CDC_TXRX_EPSIZE,
	0x00,
	0x00,
	/* Bulk out ENDPOINT*/
	7,
	DTYPE_Endpoint,
	CDC_RX_EPADDR,
	EP_TYPE_BULK,
	CDC_TXRX_EPSIZE,
	0x00,
	0x00,
};

volatile uint32_t msTicks; /* counts 1ms timeTicks */
/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
	INT_Disable();
	msTicks++;       /* increment counter necessary in Delay()*/
	INT_Enable();
}



/** Contains the current baud rate and other settings of the virtual serial port. While this demo does not use
 *  the physical USART and thus does not use these settings, they must still be retained and returned to the host
 *  upon request or the host will assume the device is non-functional.
 *
 *  These values are set by the host via a class-specific request, however they are not required to be used accurately.
 *  It is possible to completely ignore these value or use other settings as the host is completely unaware of the physical
 *  serial link characteristics and instead sends and receives data in endpoint streams.
 */
static CDC_LineEncoding_t LineEncoding = { .BaudRateBPS = 0,
                                           .CharFormat  = CDC_LINEENCODING_OneStopBit,
                                           .ParityType  = CDC_PARITY_None,
                                           .DataBits    = 8
                                         };

//static uint8_t notifications[] = {0xA1, 0x20, 0, 0, 0, 0, 2, 0, 0, 0};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	for (;;) {
		// VCOM_Echo();
		VCOM_Bridge();
	}
}


static void UartConfiguration(CDC_LineEncoding_t *LineCoding)
{
	uint32_t frame = 0;
	switch (LineCoding->DataBits) {
	case 5:
		frame |= UART_FRAME_DATABITS_FIVE;
		break;
	case 6:
		frame |= UART_FRAME_DATABITS_SIX;
		break;
	case 7:
		frame |= UART_FRAME_DATABITS_SEVEN;
		break;
	case 8:
		frame |= UART_FRAME_DATABITS_EIGHT;
		break;
	case 9:
		frame |= UART_FRAME_DATABITS_SIXTEEN;
		break;
	default:
		return;
	}

	switch (LineCoding->ParityType) {
	case 0:
		frame |= UART_FRAME_PARITY_NONE;
		break;
	case 1:
		frame |= UART_FRAME_PARITY_ODD;
		break;
	case 2:
		frame |= UART_FRAME_PARITY_EVEN;
		break;
	default:
		return;
	}

	switch (LineCoding->CharFormat) {
	case 0:
		frame |= UART_FRAME_STOPBITS_ONE;
		break;
	case 1:
		frame |= UART_FRAME_STOPBITS_ONEANDAHALF;
		break;
	case 2:
		frame |= UART_FRAME_STOPBITS_TWO;
		break;
	default:
		return;
	}
    /* Program new UART baudrate etc. */
    UART_PORT->FRAME = frame;
    USART_BaudrateAsyncSet(UART_PORT, 0, LineCoding->BaudRateBPS, usartOVS16);
}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Chip errata */
	CHIP_Init();
#if (BOARD == BOARD_DK3750)
	BSP_Init(BSP_INIT_DEFAULT);   /* Initialize DK board register access */
#endif

	/* Enable HFXO */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	/* Hardware Initialization */
	setupSWOForPrint();

	/* Enable clock to GPIO, USART1*/
	CMU->HFPERCLKEN0 |= (CMU_HFPERCLKEN0_USART1 | CMU_HFPERCLKEN0_GPIO | CMU_HFPERCLKEN0_UART1);

	SystemCoreClockGet();
	SysTick_Config(SystemCoreClockGet() / SYSTICKHZ);
	Buttons_Init();
	LEDs_Init();
	SerialPortInit();

	USB_Init(EndpointDescriptors);
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops the USB management and CDC management tasks.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the CDC management task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup CDC Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, CDC_NOTIFICATION_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_RX_EPADDR, EP_TYPE_BULK,  CDC_TXRX_EPSIZE, 1);

	/* Reset line encoding baud rate so that the host knows to send new values */
	LineEncoding.BaudRateBPS = 0;

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);

	/* Set BULK out endpoint enabled ready to receive data */
	Endpoint_SelectEndpoint(CDC_RX_EPADDR);
	Endpoint_ClearOUT();
}


/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
bool EVENT_USB_Device_ControlRequest(void)
{
	bool ret = false;
	/* Process CDC specific control requests */
	switch (USB_ControlRequest.bRequest) {
	case CDC_REQ_GetLineEncoding: // 0x21
		if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
			Endpoint_ClearSETUP();

			/* Write the line coding data to the control endpoint */
			Endpoint_Write_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
			Endpoint_ClearOUT();
			ret = true;
		}

		break;
	case CDC_REQ_SetLineEncoding: // 0x20
		if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {

			Endpoint_ClearSETUP();
			/* Read the line coding data in from the host into the global struct */
			Endpoint_Read_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
			/* The Line coding data next to setup packet. We cannot rewind buffer
			   pointer before it read out from buffer. */
			Endpoint_ClearIN();
			UartConfiguration(&LineEncoding);
			ret = true;
		}

		break;
	case CDC_REQ_SetControlLineState: // 0x22
		if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
			Endpoint_ClearSETUP();
			Endpoint_ClearStatusStage();

			// EVENT_USB_Device_SetControlLineState();
			// Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPADDR);
			// Endpoint_Write_Stream_LE(&notifications, sizeof(notifications), NULL);
			// Endpoint_ClearIN();
			// while(!Endpoint_IsINReady());
			/* NOTE: Here you can read in the line state mask from the host, to get the current state of the output handshake
			         lines. The mask is read in from the wValue parameter in USB_ControlRequest, and can be masked against the
					 CONTROL_LINE_OUT_* masks to determine the RTS and DTR line states using the following code:
			*/
			ret = true;
		}
		break;
	}
	return ret;
}

/** Function to manage CDC data transmission and reception to and from the host. */
void VCOM_Echo(void)
{
	uint8_t tmp;
	/* Select the Serial Rx Endpoint */

	while (1) {
		Endpoint_SelectEndpoint(CDC_RX_EPADDR);
		if (Endpoint_IsOUTReceived()) {
			tmp = Endpoint_Read_8();
			Endpoint_ClearOUT();
			Endpoint_SelectEndpoint(CDC_TX_EPADDR);
			Endpoint_Write_8(tmp);
			Endpoint_ClearIN();
			while (!Endpoint_IsINReady());
		}
	}
}

void VCOM_Bridge(void)
{
	uint8_t tmp;
	USART_TypeDef           *uart = UART_PORT;
	/* Select the Serial Rx Endpoint */

	while (1) {
		Endpoint_SelectEndpoint(CDC_RX_EPADDR);
		if (Endpoint_IsOUTReceived()) {
			tmp = Endpoint_Read_8();
			Endpoint_ClearOUT();
			USART_Tx(uart, tmp);
		}
		if (uart->STATUS & USART_STATUS_RXDATAV) {
			tmp = uart->RXDATA;
			Endpoint_SelectEndpoint(CDC_TX_EPADDR);
			Endpoint_Write_8(tmp);
			Endpoint_ClearIN();
			while (!Endpoint_IsINReady());
		}
	}
}
