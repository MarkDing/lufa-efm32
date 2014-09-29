/**************************************************************************//**
 * @file retarget.c
 * @brief Sample firmware for use with EFM32 Demo Programmer
 * @author Silicon Labs
 * @version 1.00
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#include "em_device.h"
#include "em_gpio.h"

/******************************************************************************
 *
 * This firmware will blink an LED on any EFM32 starter kit.
 *
 ******************************************************************************/

int RETARGET_WriteChar(char c)
{
	return ITM_SendChar(c);
}

int RETARGET_ReadChar(void)
{
	return 0;
}


/* Make binary larger so we can measure transfer speed more accurately.
 * This requires the firmware_end section to be defined in linker file,
 * see blink.icf.
 */

void setupSWOForPrint(void)
{
	/* Enable GPIO clock. */
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;

	/* Enable Serial wire output pin */
	GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;

#if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_LEOPARD_FAMILY) || defined(_EFM32_WONDER_FAMILY)
	/* Set location 0 */
	GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;

	/* Enable output on pin - GPIO Port F, Pin 2 */
	GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
	GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;
#else
	/* Set location 1 */
	GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC1;
	/* Enable output on pin */
	GPIO->P[2].MODEH &= ~(_GPIO_P_MODEH_MODE15_MASK);
	GPIO->P[2].MODEH |= GPIO_P_MODEH_MODE15_PUSHPULL;
#endif

	/* Enable debug clock AUXHFRCO */
	CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

	/* Wait until clock is ready */
	while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY));

	/* Enable trace in core debug */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	ITM->LAR  = 0xC5ACCE55;
	ITM->TER  = 0x0;
	ITM->TCR  = 0x0;
	TPI->SPPR = 2;
	TPI->ACPR = 0xf;
	ITM->TPR  = 0x0;
	DWT->CTRL = 0x400003FE;
	ITM->TCR  = 0x0001000D;
	TPI->FFCR = 0x00000100;
	ITM->TER  = 0x1;
}


/***************************************************************************//**
 * @addtogroup RetargetIo
 * @{ This module provide low-level stubs for retargetting stdio for all
 *    supported toolchains.
 *    The stubs are minimal yet sufficient implementations.
 *    Refer to chapter 12 in the reference manual for newlib 1.17.0
 *    for details on implementing newlib stubs.
 ******************************************************************************/


#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em_device.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
int fileno(FILE *);
/** @endcond */

int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
caddr_t _sbrk(int incr);
int _write(int file, const char *ptr, int len);

extern char _end;                 /**< Defined by the linker */

/**************************************************************************//**
 * @brief
 *  Close a file.
 *
 * @param[in] file
 *  File you want to close.
 *
 * @return
 *  Returns 0 when the file is closed.
 *****************************************************************************/
int _close(int file)
{
	(void) file;
	return 0;
}

/**************************************************************************//**
 * @brief Exit the program.
 * @param[in] status The value to return to the parent process as the
 *            exit status (not used).
 *****************************************************************************/
void _exit(int status)
{
	(void) status;
	while (1) {}      /* Hang here forever... */
}

/**************************************************************************//**
 * @brief
 *  Status of an open file.
 *
 * @param[in] file
 *  Check status for this file.
 *
 * @param[in] st
 *  Status information.
 *
 * @return
 *  Returns 0 when st_mode is set to character special.
 *****************************************************************************/
int _fstat(int file, struct stat *st)
{
	(void) file;
	st->st_mode = S_IFCHR;
	return 0;
}

/**************************************************************************//**
 * @brief Get process ID.
 *****************************************************************************/
int _getpid(void)
{
	return 1;
}

/**************************************************************************//**
 * @brief
 *  Query whether output stream is a terminal.
 *
 * @param[in] file
 *  Descriptor for the file.
 *
 * @return
 *  Returns 1 when query is done.
 *****************************************************************************/
int _isatty(int file)
{
	(void) file;
	return 1;
}

/**************************************************************************//**
 * @brief Send signal to process.
 * @param[in] pid Process id (not used).
 * @param[in] sig Signal to send (not used).
 *****************************************************************************/
int _kill(int pid, int sig)
{
	(void)pid;
	(void)sig;
	return -1;
}

/**************************************************************************//**
 * @brief
 *  Set position in a file.
 *
 * @param[in] file
 *  Descriptor for the file.
 *
 * @param[in] ptr
 *  Poiter to the argument offset.
 *
 * @param[in] dir
 *  Directory whence.
 *
 * @return
 *  Returns 0 when position is set.
 *****************************************************************************/
int _lseek(int file, int ptr, int dir)
{
	(void) file;
	(void) ptr;
	(void) dir;
	return 0;
}

/**************************************************************************//**
 * @brief
 *  Read from a file.
 *
 * @param[in] file
 *  Descriptor for the file you want to read from.
 *
 * @param[in] ptr
 *  Pointer to the chacaters that are beeing read.
 *
 * @param[in] len
 *  Number of characters to be read.
 *
 * @return
 *  Number of characters that have been read.
 *****************************************************************************/
int _read(int file, char *ptr, int len)
{
	int c, rxCount = 0;

	(void) file;

	while (len--) {
		if ((c = RETARGET_ReadChar()) != -1) {
			*ptr++ = c;
			rxCount++;
		} else {
			break;
		}
	}

	if (rxCount <= 0) {
		return -1;                        /* Error exit */
	}

	return rxCount;
}

/**************************************************************************//**
 * @brief
 *  Increase heap.
 *
 * @param[in] incr
 *  Number of bytes you want increment the program's data space.
 *
 * @return
 *  Rsturns a pointer to the start of the new area.
 *****************************************************************************/
caddr_t _sbrk(int incr)
{
	static char       *heap_end;
	char              *prev_heap_end;
	static const char heaperr[] = "Heap and stack collision\n";

	if (heap_end == 0) {
		heap_end = &_end;
	}

	prev_heap_end = heap_end;
	if ((heap_end + incr) > (char *) __get_MSP()) {
		_write(fileno(stdout), heaperr, strlen(heaperr));
		exit(1);
	}
	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

/**************************************************************************//**
 * @brief
 *  Write to a file.
 *
 * @param[in] file
 *  Descriptor for the file you want to write to.
 *
 * @param[in] ptr
 *  Pointer to the text you want to write
 *
 * @param[in] len
 *  Number of characters to be written.
 *
 * @return
 *  Number of characters that have been written.
 *****************************************************************************/
int _write(int file, const char *ptr, int len)
{
	int txCount;

	(void) file;

	for (txCount = 0; txCount < len; txCount++) {
		RETARGET_WriteChar(*ptr++);
	}

	return len;
}
#endif /* !defined( __CROSSWORKS_ARM ) && defined( __GNUC__ ) */

#if defined(__ICCARM__)
#include <yfuns.h>
#include <stdint.h>

_STD_BEGIN

/**************************************************************************//**
 * @brief Transmit buffer to USART1
 * @param buffer Array of characters to send
 * @param nbytes Number of bytes to transmit
 * @return Number of bytes sent
 *****************************************************************************/
static int TxBuf(uint8_t *buffer, int nbytes)
{
	int i;

	for (i = 0; i < nbytes; i++) {
		RETARGET_WriteChar(*buffer++);
	}
	return nbytes;
}

/*
 * If the __write implementation uses internal buffering, uncomment
 * the following line to ensure that we are called with "buffer" as 0
 * (i.e. flush) when the application terminates.
 */

size_t __write(int handle, const unsigned char *buffer, size_t size)
{
	/* Remove the #if #endif pair to enable the implementation */

	size_t nChars = 0;

	if (buffer == 0) {
		/*
		 * This means that we should flush internal buffers.  Since we
		 * don't we just return.  (Remember, "handle" == -1 means that all
		 * handles should be flushed.)
		 */
		return 0;
	}

	/* This template only writes to "standard out" and "standard err",
	 * for all other file handles it returns failure. */
	if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
		return _LLIO_ERROR;
	}

	/* Hook into USART1 transmit function here */
	if (TxBuf((uint8_t *) buffer, size) != size)
		return _LLIO_ERROR;
	else
		nChars = size;

	return nChars;
}

size_t __read(int handle, unsigned char *buffer, size_t size)
{
	/* Remove the #if #endif pair to enable the implementation */
	int nChars = 0;

	/* This template only reads from "standard in", for all other file
	 * handles it returns failure. */
	if (handle != _LLIO_STDIN) {
		return _LLIO_ERROR;
	}

	for (/* Empty */; size > 0; --size) {
		int c = RETARGET_ReadChar();
		if (c < 0)
			break;

		*buffer++ = c;
		++nChars;
	}

	return nChars;
}

_STD_END

#endif /* defined( __ICCARM__ ) */

#if defined(__CROSSWORKS_ARM)

/* Pass each of these function straight to the USART */
int __putchar(int ch)
{
	return (RETARGET_WriteChar(ch));
}

int __getchar(void)
{
	return (RETARGET_ReadChar());
}

#endif /* defined( __CROSSWORKS_ARM ) */

#if defined(__CC_ARM)

#include <stdio.h>

/* #pragma import(__use_no_semihosting_swi) */

struct __FILE {
	int handle;
};

/**Standard output stream*/
FILE __stdout;

/**************************************************************************//**
 * @brief
 *  Writes character to file
 *
 * @param[in] f
 *  File
 *
 * @param[in] ch
 *  Character
 *
 * @return
 *  Written character
 *****************************************************************************/
int fputc(int ch, FILE *f)
{
	return (RETARGET_WriteChar(ch));
}

/**************************************************************************//**
 * @brief
 *  Reads character from file
 *
 * @param[in] f
 *  File
 *
 * @return
 *  Character
 *****************************************************************************/
int fgetc(FILE *f)
{
	return (RETARGET_ReadChar());
}

/**************************************************************************//**
 * @brief
 *  Tests the error indicator for the stream pointed
 *  to by file
 *
 * @param[in] f
 *  File
 *
 * @return
 *  Returns non-zero if it is set
 *****************************************************************************/
int ferror(FILE *f)
{
	/* Your implementation of ferror */
	return EOF;
}

/**************************************************************************//**
 * @brief
 *  Writes a character to the console
 *
 * @param[in] ch
 *  Character
 *****************************************************************************/
void _ttywrch(int ch)
{
	RETARGET_WriteChar(ch);
}

/**************************************************************************//**
 * @brief
 *  Library exit function. This function is called if stack
 *  overflow occurs.
 *
 * @param[in] return_code
 *  Return code
 *****************************************************************************/
void _sys_exit(int return_code)
{
label:  goto label; /* endless loop */
}
#endif /* defined( __CC_ARM ) */

/** @} (end group RetargetIo) */
