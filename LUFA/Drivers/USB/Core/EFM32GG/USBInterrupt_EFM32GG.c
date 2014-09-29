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

#include "../../../../Common/Common.h"
#if (ARCH == ARCH_EFM32GG)

#define  __INCLUDE_FROM_USB_DRIVER
#include "../USBInterrupt.h"

#define HANDLE_INT( x ) if ( status & x ) { Handle_##x(); status &= ~x; }

static void Handle_USB_GINTSTS_ENUMDONE(void);
static void Handle_USB_GINTSTS_IEPINT(void);
static void Handle_USB_GINTSTS_OEPINT(void);
static void Handle_USB_GINTSTS_RESETDET(void);
static void Handle_USB_GINTSTS_SOF(void);
static void Handle_USB_GINTSTS_USBRST(void);
static void Handle_USB_GINTSTS_USBSUSP(void);
static void Handle_USB_GINTSTS_WKUPINT(void);

void USB_INT_DisableAllInterrupts(void)
{
	/* Disable all device interrupts */
	USB->DIEPMSK  = 0;
	USB->DOEPMSK  = 0;
	USB->DAINTMSK = 0;
	USB->DIEPEMPMSK = 0;
	USB->GINTMSK = 0;
}

void USB_INT_ClearAllInterrupts(void)
{
	uint8_t i;
	for (i = 0; i <= MAX_NUM_IN_EPS; i++) {
		USB_DINEPS[i].INT  = 0xFFFFFFFF;
		USB_DOUTEPS[i].INT  = 0xFFFFFFFF;
	}
	USB->GINTSTS = 0xFFFFFFFF;
}

/*
 * USB_IRQHandler() is the first level handler for the USB peripheral interrupt.
 */
void USB_IRQHandler(void)
{
	uint32_t status;

	INT_Disable();

	if (USB->IF && (USB->CTRL & USB_CTRL_VREGOSEN)) {
		if (USB->IF & USB_IF_VREGOSH) {
			USB->IFC = USB_IFC_VREGOSH;
			if (USB->STATUS & USB_STATUS_VREGOS) {
				USBDHAL_EnableUsbResetAndSuspendInt();
				USB_DeviceState = DEVICE_STATE_Powered;
			}
		}
		if (USB->IF & USB_IF_VREGOSL) {
			USB->IFC = USB_IFC_VREGOSL;
			if (!(USB->STATUS & USB_STATUS_VREGOS))	{
				USB->GINTMSK = 0;
				USB->GINTSTS = 0xFFFFFFFF;
				USB_DeviceState = DEVICE_STATE_Unattached;
			}
		}
	}

	status = USBHAL_GetCoreInts();
	// printf("\nGINTSTS = 0x%x\n", status);

	if (status == 0) {
		USBDHAL_Ep0Activate(0);
		INT_Enable();
		return;
	}

	HANDLE_INT(USB_GINTSTS_RESETDET)
	HANDLE_INT(USB_GINTSTS_WKUPINT)
	HANDLE_INT(USB_GINTSTS_USBSUSP)
	HANDLE_INT(USB_GINTSTS_SOF)
	HANDLE_INT(USB_GINTSTS_ENUMDONE)
	HANDLE_INT(USB_GINTSTS_USBRST)
	HANDLE_INT(USB_GINTSTS_IEPINT)
	HANDLE_INT(USB_GINTSTS_OEPINT)

	USBDHAL_Ep0Activate(0);
	INT_Enable();
}

/*
 * Handle port enumeration interrupt. This has nothing to do with normal
 * device enumeration.
 */
static void Handle_USB_GINTSTS_ENUMDONE(void)
{
	USB->GINTSTS = USB_GINTSTS_ENUMDONE;
	dev->ep[0].state = D_EP_IDLE;
	USBDHAL_EnableInts(dev);
}


/*
 * Handle IN endpoint transfer interrupt.
 */
static void Handle_USB_GINTSTS_IEPINT(void)
{
	int epnum;
	uint16_t epint;
	uint16_t epmask;
	USBD_Ep_TypeDef *ep;


	epint = USBDHAL_GetAllInEpInts();
	for (epnum = 0, epmask = 1; epnum <= NUM_EP_USED; epnum++, epmask <<= 1) {
		if (epint & epmask) {
			ep = &dev->ep[epnum];
			USBDHAL_GetInEpInts(ep);
		}
	}
}

/*
 * Handle OUT endpoint transfer interrupt.
 */
static void Handle_USB_GINTSTS_OEPINT(void)
{
	int epnum;
	uint16_t epint;
	uint16_t epmask;
	uint32_t status;
	USBD_Ep_TypeDef *ep;

	epint = USBDHAL_GetAllOutEpInts();
	for (epnum = 0, epmask = 1; epnum <= NUM_EP_USED; epnum++, epmask <<= 1) {
		if (epint & epmask) {
			ep = USBD_GetEpFromAddr(epnum);
			status = USBDHAL_GetOutEpInts(ep);
			/* Setup Phase Done */
			if (status & USB_DOEP_INT_SETUP) {
				uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
				USB_Device_ProcessControlRequest();
				Endpoint_SelectEndpoint(PrevSelectedEndpoint);
			}
		}
	}
}

/*
 * Handle USB reset detectet interrupt in suspend mode.
 */
static void Handle_USB_GINTSTS_RESETDET(void)
{
	USB->GINTSTS = USB_GINTSTS_RESETDET;
	USB_DeviceState = DEVICE_STATE_Default;
}

/*
 * Handle Start Of Frame (SOF) interrupt.
 */
static void Handle_USB_GINTSTS_SOF(void)
{
	USB->GINTSTS = USB_GINTSTS_SOF;

	if (dev->callbacks->sofInt) {
		dev->callbacks->sofInt(
		    (USB->DSTS & _USB_DSTS_SOFFN_MASK) >> _USB_DSTS_SOFFN_SHIFT);
	}
}

/*
 * Handle USB port reset interrupt.
 */
static void Handle_USB_GINTSTS_USBRST(void)
{
	int i;

	/* Clear Remote Wakeup Signalling */
	USB->DCTL &= ~(DCTL_WO_BITMASK | USB_DCTL_RMTWKUPSIG);
	USBHAL_FlushTxFifo(0);

	/* Clear pending interrupts */
	for (i = 0; i <= MAX_NUM_IN_EPS; i++) {
		USB_DINEPS[ i ].INT = 0xFFFFFFFF;
	}

	for (i = 0; i <= MAX_NUM_OUT_EPS; i++) {
		USB_DOUTEPS[ i ].INT = 0xFFFFFFFF;
	}

	USB->DAINTMSK = USB_DAINTMSK_INEPMSK0 | USB_DAINTMSK_OUTEPMSK0;
	USB->DOEPMSK  = USB_DOEPMSK_SETUPMSK  | USB_DOEPMSK_XFERCOMPLMSK;
	USB->DIEPMSK  = USB_DIEPMSK_XFERCOMPLMSK;

	/* Reset Device Address */
	USB->DCFG &= ~_USB_DCFG_DEVADDR_MASK;

	/* Setup EP0 to receive SETUP packets */
	USB->DOEP0TSIZ = 3 << _USB_DOEP0TSIZ_SUPCNT_SHIFT;
	USB->DOEP0DMAADDR = (uint32_t)ep->buf;
	USB->DOEP0CTL = (USB->DOEP0CTL & ~DEPCTL_WO_BITMASK) |
	                USB_DOEP_CTL_CNAK | USB_DOEP_CTL_EPENA;

	USBDHAL_EnableInts(dev);
	USB->GINTMSK |= USB_GINTMSK_SOFMSK;
	if (dev->callbacks->usbReset) {
		dev->callbacks->usbReset();
	}

	USB_DeviceState = DEVICE_STATE_Default;
	// USBDHAL_AbortAllTransfers(USB_STATUS_DEVICE_RESET);
}

/*
 * Handle USB port suspend interrupt.
 */
static void Handle_USB_GINTSTS_USBSUSP(void)
{
	USB->GINTSTS = USB_GINTSTS_USBSUSP;
	USB_DeviceState = DEVICE_STATE_Suspended;
}

/*
 * Handle USB port wakeup interrupt.
 */
static void Handle_USB_GINTSTS_WKUPINT(void)
{
	USB->GINTSTS = USB_GINTSTS_WKUPINT;

	if (USB_Device_ConfigurationNumber)
		USB_DeviceState = DEVICE_STATE_Configured;
	else
		USB_DeviceState = (USB_Device_IsAddressSet()) ? DEVICE_STATE_Addressed :
		                  DEVICE_STATE_Powered;
}

#if defined(INTERRUPT_CONTROL_ENDPOINT) && defined(USB_CAN_BE_DEVICE)
ISR(USB_COM_vect)
{
	uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();

	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
	USB_INT_Disable(USB_INT_RXSTPI);

	GlobalInterruptEnable();

	USB_Device_ProcessControlRequest();

	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
	USB_INT_Enable(USB_INT_RXSTPI);
	Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}
#endif

#endif
