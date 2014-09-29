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
#define  __INCLUDE_FROM_USB_CONTROLLER_C
#include "../USBController.h"

#define MAX_FIFO_SIZE 512U
static USBD_Device_TypeDef device;
USBD_Device_TypeDef *dev = &device;

USBD_Ep_TypeDef *ep;

/* Define callbacks that are called by the USB stack on different events. */
static const USBD_Callbacks_TypeDef callbacks = {
	.usbReset        = NULL,              /* Called whenever USB reset signalling is detected on the USB port. */
	.usbStateChange  = NULL,       /* Called whenever the device change state.  */
	.setupCmd        = NULL,              /* Called on each setup request received from host. */
	.isSelfPowered   = NULL,              /* Called whenever the device stack needs to query if the device is currently self- or bus-powered. */
	.sofInt          = NULL               /* Called at each SOF (Start of Frame) interrupt. If NULL, the device stack will not enable the SOF interrupt. */
};


static void USB_Fifo_Init(uint8_t *endpoint_desc)
{
	USB_StdDescriptor_Endpoint_t *epd;
	uint8_t *config;
	uint8_t *buf = receiveBuffer;
	uint8_t txFifoNum = 0, Multiplier = 1;
	uint16_t start = 0, depth = 0;
	uint32_t totalRxFifoSize = 0, totalTxFifoSize = 0;
	uint32_t numEps = 0, numInEps = 0, numOutEps = 0;
	int i, j;
	USBD_Ep_TypeDef *ep;

	config = &endpoint_desc[1];
	epd = (USB_StdDescriptor_Endpoint_t *)config;
	do {

		ep                 = &dev->ep[numEps++];
		ep->in             = (epd->bEndpointAddress & USB_SETUP_DIR_MASK) != 0;
		ep->buf            = buf;
		ep->addr           = epd->bEndpointAddress;
		ep->num            = ep->addr & USB_EPNUM_MASK;
		ep->mask           = 1 << ep->num;
		ep->type           = epd->bmAttributes & CONFIG_DESC_BM_TRANSFERTYPE;
		ep->packetSize     = epd->wMaxPacketSize;
		ep->remaining      = 0;
		ep->xferred        = 0;
		ep->state          = D_EP_IDLE;
		ep->xferCompleteCb = NULL;

		buf += ep->packetSize;

		if (ep->type == EP_TYPE_BULK)
			Multiplier = 2;
		else
			Multiplier = 1;

		if (ep->in) {
			numInEps++;
			ep->txFifoNum = txFifoNum++;
			ep->fifoSize = (ep->packetSize / 4) * Multiplier;
			dev->inEpAddr2EpIndex[ep->num] = numEps;
			totalTxFifoSize += ep->fifoSize;
		} else {
			if (ep->type == EP_TYPE_CONTROL) {
				ep->fifoSize = ep->packetSize / 4;
			} else {
				numOutEps++;
				ep->fifoSize = (ep->packetSize / 4 + 1) * Multiplier;
				dev->outEpAddr2EpIndex[ep->num] = numEps;
			}
			totalRxFifoSize += ep->fifoSize;
		}
		epd++;
	} while (numEps < endpoint_desc[0]);
	/* Rx-FIFO size: SETUP packets : 4*n + 6    n=#CTRL EP's
	 *               GOTNAK        : 1
	 *               Status info   : 2*n        n=#OUT EP's (EP0 included) in HW
	 */
	totalRxFifoSize += 10 + 1 + (2 * (MAX_NUM_OUT_EPS + 1));

	/* Set Rx FIFO size */
	USB->GRXFSIZ = (totalRxFifoSize << _USB_GRXFSIZ_RXFDEP_SHIFT) &
	               _USB_GRXFSIZ_RXFDEP_MASK;

	start = totalRxFifoSize;                /* Set Tx EP0 FIFO size */
	depth = dev->ep[0].fifoSize;
	USB->GNPTXFSIZ = ((depth << _USB_GNPTXFSIZ_NPTXFINEPTXF0DEP_SHIFT) &
	                  _USB_GNPTXFSIZ_NPTXFINEPTXF0DEP_MASK) |
	                 ((start << _USB_GNPTXFSIZ_NPTXFSTADDR_SHIFT) &
	                  _USB_GNPTXFSIZ_NPTXFSTADDR_MASK);


	/* Set Tx EP FIFO sizes for all IN ep's */
	for (j = 1; j <= MAX_NUM_TX_FIFOS; j++) {
		for (i = 1; i <= MAX_NUM_IN_EPS; i++) {
			ep = USBD_GetEpFromAddr(USB_SETUP_DIR_MASK | i);
			if (ep) {                             /* Is EP in use ? */
				if (ep->txFifoNum == j) {           /* Is it correct FIFO number ? */
					start += depth;
					depth = ep->fifoSize;
					USB_DIEPTXFS[ep->txFifoNum - 1] =
					    (depth << _USB_DIEPTXF1_INEPNTXFDEP_SHIFT) |
					    (start &  _USB_DIEPTXF1_INEPNTXFSTADDR_MASK);
				}
			}
		}
	}

	if (totalRxFifoSize + totalTxFifoSize > MAX_FIFO_SIZE)
		return;

	/* Flush the FIFO's */
	USBHAL_FlushTxFifo(0x10);        /* All Tx FIFO's */
	USBHAL_FlushRxFifo();            /* The Rx FIFO   */
}

void USB_Init(uint8_t *endpoint_desc)
{
	USB_Disable();

	USB_IsInitialized = true;

	memset(dev, 0, sizeof(USBD_Device_TypeDef));
	dev->callbacks = &callbacks;
	dev->setup = dev->setupPkt;

	/* Initialize EP0 */
	ep = &dev->ep[0];

	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	/* Enable USB clock */
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC;

	CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_HFCLK);
	USBHAL_DisableGlobalInt();

	USB_ResetInterface();
	USB_Fifo_Init(endpoint_desc);
	USB_Init_Device();

	USBHAL_EnableGlobalInt();
	NVIC_ClearPendingIRQ(USB_IRQn);
	NVIC_EnableIRQ(USB_IRQn);
	INT_Enable();
}

void USB_Disable(void)
{
	USB_INT_DisableAllInterrupts();
	USB_INT_ClearAllInterrupts();

	USB_Detach();
	USB_Controller_Disable();

	USB_IsInitialized = false;
}


void USB_ResetInterface(void)
{
	USB_INT_DisableAllInterrupts();
	USB_INT_ClearAllInterrupts();

	USB_Controller_Enable();  /* Init PHY          */
	USB_Controller_Reset();
}


static void USB_Init_Device(void)
{
	USB_DeviceState                 = DEVICE_STATE_Unattached;
	USB_Device_ConfigurationNumber  = 0;

	USB_Device_RemoteWakeupEnabled  = false;
	USB_Device_CurrentlySelfPowered = false;

	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 0);    // Enable VBUSEN pin

	/* Force Device Mode */
	USB->GUSBCFG = (USB->GUSBCFG                                    &
	                ~(GUSBCFG_WO_BITMASK | USB_GUSBCFG_FORCEHSTMODE)) |
	               USB_GUSBCFG_FORCEDEVMODE;
	INT_Enable();
	Delay_MS(50);
	INT_Disable();

	/* Set device speed */
	USB_Device_SetFullSpeed();

	/* Stall on non-zero len status OUT packets (ctrl transfers). */
	USB->DCFG |= USB_DCFG_NZSTSOUTHSHK;

	/* Set periodic frame interval to 80% */
	USB->DCFG &= ~_USB_DCFG_PERFRINT_MASK;

	/* Set DMA enabled and incrementing burst of unspecified length*/
	USB->GAHBCFG = (USB->GAHBCFG & ~_USB_GAHBCFG_HBSTLEN_MASK) |
	               USB_GAHBCFG_DMAEN | USB_GAHBCFG_HBSTLEN_INCR;

	/* Enable VREGO sense. */
	USB->CTRL |= USB_CTRL_VREGOSEN;

	USB_INT_Enable(USB_INT_VREGOSH);
	USB_INT_Enable(USB_INT_VREGOSL);
	/* Force a VREGO interrupt. */
	if (USB->STATUS & USB_STATUS_VREGOS)
		USB->IFS = USB_IFS_VREGOSH;
	else
		USB->IFS = USB_IFS_VREGOSL;

	Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
	                           USB_Device_ControlEndpointSize, 1);

	USB_INT_Enable(USB_GINT_USBSUSP);
	USB_INT_Enable(USB_GINT_SOF);
	USB_Attach();
}

#endif
