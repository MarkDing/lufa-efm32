/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
 *  \brief LUFA Custom Board Serial Port Hardware Driver (Template)
 *
 *  This is a stub driver header file, for implementing custom board
 *  layout hardware with compatible LUFA board specific drivers. If
 *  the library is configured to use the BOARD_USER board mode, this
 *  driver file should be completed and copied into the "/Board/" folder
 *  inside the application's folder.
 *
 *  This stub is for the board-specific component of the LUFA Buttons driver,
 *  for the control of physical board-mounted GPIO pushbuttons.
 */

#ifndef __SERIAL_USER_H__
#define __SERIAL_USER_H__

/* Includes: */
// TODO: Add any required includes here
#include "em_usart.h"
/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/* Preprocessor Checks: */
#if !defined(__INCLUDE_FROM_SERIAL_H)
#error Do not include this file directly. Include LUFA/Drivers/Board/Serial.h instead.
#endif

/* Public Interface - May be used in end-application: */
/* Macros: */
/** UART PORT in STK3700 is USART1 */
#define UART_PORT          USART1

/* Inline Functions: */
#if !defined(__DOXYGEN__)
/**************************************************************************//**
 * @brief Initialize the UART peripheral.
 *****************************************************************************/
static inline void SerialPortInit(void)
{
	USART_TypeDef          *usart = UART_PORT;
	USART_InitAsync_TypeDef init  = USART_INITASYNC_DEFAULT;

	/* Set TXD pin to push-pull, RXD pin to input.
	 To avoid false start, configure output as high */
	GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);

	/* Configure UART for basic async operation */
	init.enable = usartDisable;
	USART_InitAsync(usart, &init);

	/* Enable pins at USART1 location #1 */
	usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC1;

	/* Finally enable it */
	USART_Enable(usart, usartEnable);
}

#endif

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

#endif

