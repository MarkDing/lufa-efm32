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
 *  \brief Digital Serial board hardware driver.
 *
 *  This file is the master dispatch header file for the board-specific Serial driver, for boards containing
 *  physical pushbuttons connected to the microcontroller's GPIO pins.
 *
 *  User code should include this file, which will in turn include the correct Serial driver header file for the
 *  currently selected board.
 *
 *  If the \c BOARD value is set to \c BOARD_USER, this will include the \c /Board/Serial.h file in the user project
 *  directory.
 *
 *  For possible \c BOARD makefile values, see \ref Group_BoardTypes.
 */

/** \ingroup Group_BoardDrivers
 *  \defgroup Group_SerialPort Serial Port Driver - LUFA/Drivers/Board/Serial.h
 *  \brief Hardware Serial UART/USART driver.
 *
 *  \section Sec_SerialPort_Dependencies Module Source Dependencies
 *  The following files must be built with any user project that uses this module:
 *    - None
 *
 *  \section Sec_SerialPort_ModDescription Module Description
 *  Hardware serial USART driver. This module provides an easy to use driver for the setup and transfer
 *  of data over the selected architecture and microcontroller model's UART/USART port.
 *
 *  \note The exact API for this driver may vary depending on the target used - see
 *        individual target module documentation for the API specific to your target processor.
 *
 *  \section Sec_SerialPort_ExampleUsage Example Usage
 *  The following snippet is an example of how this module may be used within a typical
 *  application.
 *
 *  \code
 *      // Initialize the Serial driver before first use
 *      SerialPortInit();
 *
 *
 *  \endcode
 *
 *  @{
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

/* Macros: */
#define __INCLUDE_FROM_SERIAL_H

/* Includes: */
#include "../../Common/Common.h"

#if (BOARD == BOARD_NONE)
static inline void       SerialPortInit(void) {};
#elif (BOARD == BOARD_STK3700)
#include "EFM32GG/STK3700/Serial.h"
#elif (BOARD == BOARD_DK3750)
#include "EFM32GG/DK3750/Serial.h"
#else
#include "Board/Serial.h"
#endif

/* Pseudo-Functions for Doxygen: */
#if defined(__DOXYGEN__)
/** Initializes the Serial driver, USART1 for STK3700
 *
 *  This must be called before any Serial driver functions are used.
 */
static inline void SerialPortInit(void);

#endif

#endif

/** @} */

