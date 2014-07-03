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
 *  \brief USB Controller Interrupt definitions for the Giant Gecko EFM32GG microcontrollers.
 *
 *  This file contains definitions required for the correct handling of low level USB service routine interrupts
 *  from the USB controller.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB driver
 *        dispatch header located in LUFA/Drivers/USB/USB.h.
 */

#ifndef __USBINTERRUPT_EFM32GG_H__
#define __USBINTERRUPT_EFM32GG_H__

/* Includes: */
#include "../../../../Common/Common.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/* Preprocessor Checks: */
#if !defined(__INCLUDE_FROM_USB_DRIVER)
#error Do not include this file directly. Include LUFA/Drivers/USB/USB.h instead.
#endif

/* Private Interface - For use in library only: */
#if !defined(__DOXYGEN__)
/* External Variables: */

/* Enums: */
enum USB_Interrupts_t {
	USB_GINT_WKUPINT,	/* bit 31 */
	USB_GINT_RESETDET,	/* bit 23 */
	USB_GINT_OEPINT,	/* bit 19 */
	USB_GINT_IEPINT,	/* bit 18 */
	USB_GINT_ENUMDONE,	/* bit 13*/
	USB_GINT_USBRST,	/* bit 12 */
	USB_GINT_USBSUSP,	/* bit 11 */
	USB_GINT_SOF,		/* bit 3 */
	USB_INT_VREGOSH,	/* IF register */
	USB_INT_VREGOSL		/* IF register */
};

/* Inline Functions: */
static inline void USB_INT_Enable(const uint8_t Interrupt) ATTR_ALWAYS_INLINE;
static inline void USB_INT_Enable(const uint8_t Interrupt)
{
	switch (Interrupt) {
	case USB_GINT_WKUPINT:
		USB->GINTMSK |= USB_GINTMSK_WKUPINTMSK;
		break;
	case USB_GINT_RESETDET:
		USB->GINTMSK |= USB_GINTMSK_RESETDETMSK;
		break;
	case USB_GINT_OEPINT:
		USB->GINTMSK |= USB_GINTMSK_OEPINTMSK;
		break;
	case USB_GINT_IEPINT:
		USB->GINTMSK |= USB_GINTMSK_IEPINTMSK;
		break;
	case USB_GINT_ENUMDONE:
		USB->GINTMSK |= USB_GINTMSK_ENUMDONEMSK;
		break;
	case USB_GINT_USBRST:
		USB->GINTMSK |= USB_GINTMSK_USBRSTMSK;
		break;
	case USB_GINT_USBSUSP:
		USB->GINTMSK |= USB_GINTMSK_USBSUSPMSK;
		break;
	case USB_GINT_SOF:
		USB->GINTMSK |= USB_GINTMSK_SOFMSK;
		break;
	case USB_INT_VREGOSH:
		USB->IFC   |= USB_IFC_VREGOSH;
		USB->IEN   |= USB_IEN_VREGOSH;
		break;
	case USB_INT_VREGOSL:
		USB->IFC   |= USB_IFC_VREGOSL;
		USB->IEN   |= USB_IEN_VREGOSL;
		break;
	default:
		break;
	}
}

static inline void USB_INT_Disable(const uint8_t Interrupt) ATTR_ALWAYS_INLINE;
static inline void USB_INT_Disable(const uint8_t Interrupt)
{
	switch (Interrupt) {
	case USB_GINT_WKUPINT:
		USB->GINTMSK &= ~USB_GINTMSK_WKUPINTMSK;
		break;
	case USB_GINT_RESETDET:
		USB->GINTMSK &= ~USB_GINTMSK_RESETDETMSK;
		break;
	case USB_GINT_OEPINT:
		USB->GINTMSK &= ~USB_GINTMSK_OEPINTMSK;
		break;
	case USB_GINT_IEPINT:
		USB->GINTMSK &= ~USB_GINTMSK_IEPINTMSK;
		break;
	case USB_GINT_ENUMDONE:
		USB->GINTMSK &= ~USB_GINTMSK_ENUMDONEMSK;
		break;
	case USB_GINT_USBRST:
		USB->GINTMSK &= ~USB_GINTMSK_USBRSTMSK;
		break;
	case USB_GINT_USBSUSP:
		USB->GINTMSK &= ~USB_GINTMSK_USBSUSPMSK;
		break;
	case USB_GINT_SOF:
		USB->GINTMSK &= ~USB_GINTMSK_SOFMSK;
		break;
	case USB_INT_VREGOSH:
		USB->IEN &= ~USB_IEN_VREGOSH;
		break;
	case USB_INT_VREGOSL:
		USB->IEN &= ~USB_IEN_VREGOSL;
		break;
	default:
		break;
	}
}

static inline void USB_INT_Clear(const uint8_t Interrupt) ATTR_ALWAYS_INLINE;
static inline void USB_INT_Clear(const uint8_t Interrupt)
{
	switch (Interrupt) {
	case USB_GINT_WKUPINT:
		USB->GINTSTS |= USB_GINTSTS_WKUPINT;
		break;
	case USB_GINT_RESETDET:
		USB->GINTSTS |= USB_GINTSTS_RESETDET;
		break;
	case USB_GINT_OEPINT:
		USB->GINTSTS |= USB_GINTSTS_OEPINT;
		break;
	case USB_GINT_IEPINT:
		USB->GINTSTS |= USB_GINTSTS_IEPINT;
		break;
	case USB_GINT_ENUMDONE:
		USB->GINTSTS |= USB_GINTSTS_ENUMDONE;
		break;
	case USB_GINT_USBRST:
		USB->GINTSTS |= USB_GINTSTS_USBRST;
		break;
	case USB_GINT_USBSUSP:
		USB->GINTSTS |= USB_GINTSTS_USBSUSP;
		break;
	case USB_GINT_SOF:
		USB->GINTSTS |= USB_GINTSTS_SOF;
		break;
	case USB_INT_VREGOSH:
		USB->IFC = USB_IFC_VREGOSH;
		break;
	case USB_INT_VREGOSL:
		USB->IFC = USB_IFC_VREGOSL;
		break;
	default:
		break;
	}
}

static inline bool USB_INT_IsEnabled(const uint8_t Interrupt) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
static inline bool USB_INT_IsEnabled(const uint8_t Interrupt)
{
	uint32_t tmp;
	switch (Interrupt) {
	case USB_GINT_WKUPINT:
		tmp = USB->GINTMSK & USB_GINTMSK_WKUPINTMSK;
		break;
	case USB_GINT_RESETDET:
		tmp = USB->GINTMSK & USB_GINTMSK_RESETDETMSK;
		break;
	case USB_GINT_OEPINT:
		tmp = USB->GINTMSK & USB_GINTMSK_OEPINTMSK;
		break;
	case USB_GINT_IEPINT:
		tmp = USB->GINTMSK & USB_GINTMSK_IEPINTMSK;
		break;
	case USB_GINT_ENUMDONE:
		tmp = USB->GINTMSK & USB_GINTMSK_ENUMDONEMSK;
		break;
	case USB_GINT_USBRST:
		tmp = USB->GINTMSK & USB_GINTMSK_USBRSTMSK;
		break;
	case USB_GINT_USBSUSP:
		tmp = USB->GINTMSK & USB_GINTMSK_USBSUSPMSK;
		break;
	case USB_GINT_SOF:
		tmp = USB->GINTMSK & USB_GINTMSK_SOFMSK;
		break;
	case USB_INT_VREGOSH:
		tmp = USB->IEN & USB_IEN_VREGOSH;
		break;
	case USB_INT_VREGOSL:
		tmp = USB->IEN & USB_IEN_VREGOSL;
		break;
	default:
		return false;
	}
	return tmp ? true : false;
}

static inline bool USB_INT_HasOccurred(const uint8_t Interrupt) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
static inline bool USB_INT_HasOccurred(const uint8_t Interrupt)
{
	uint32_t tmp;
	switch (Interrupt) {
	case USB_GINT_WKUPINT:
		tmp = USB->GINTSTS & USB_GINTSTS_WKUPINT;
		break;
	case USB_GINT_RESETDET:
		tmp = USB->GINTSTS & USB_GINTSTS_RESETDET;
		break;
	case USB_GINT_OEPINT:
		tmp = USB->GINTSTS & USB_GINTSTS_OEPINT;
		break;
	case USB_GINT_IEPINT:
		tmp = USB->GINTSTS & USB_GINTSTS_IEPINT;
		break;
	case USB_GINT_ENUMDONE:
		tmp = USB->GINTSTS & USB_GINTSTS_ENUMDONE;
		break;
	case USB_GINT_USBRST:
		tmp = USB->GINTSTS & USB_GINTSTS_USBRST;
		break;
	case USB_GINT_USBSUSP:
		tmp = USB->GINTSTS & USB_GINTSTS_USBSUSP;
		break;
	case USB_GINT_SOF:
		tmp = USB->GINTSTS & USB_GINTSTS_SOF;
		break;
	case USB_INT_VREGOSH:
		tmp = USB->IF & USB_IF_VREGOSH;
		break;
	case USB_INT_VREGOSL:
		tmp = USB->IF & USB_IF_VREGOSL;
		break;
	default:
		return false;
	}
	return tmp ? true : false;
}

/* Includes: */
#include "../USBMode.h"
#include "../Events.h"
#include "../USBController.h"

/* Function Prototypes: */
void USB_INT_ClearAllInterrupts(void);
void USB_INT_DisableAllInterrupts(void);
#endif

/* Public Interface - May be used in end-application: */
/* Function Prototypes: */
#if defined(__DOXYGEN__)
/** Interrupt service routine handler for the USB controller ISR group. This interrupt routine <b>must</b> be
 *  linked to the entire USB controller ISR vector group inside the Giant Gecko's interrupt controller peripheral,
 *  using the user application's preferred USB controller driver.
 */
void USB_GEN_vect(void);
#else
ISR(USB_GEN_vect);
#endif

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

#endif

