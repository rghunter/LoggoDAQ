/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  SERIAL.C:  Low Level Serial Routines                                      */
/*  modified and extended by Martin Thomas                                    */
/*                                                                            */
/******************************************************************************/

#include "LPC214x.h"
#include "target.h"
#include "serial.h"

#define CR     0x0D

/* Write character to Serial Port 0 without \n -> \r\n  */
int putc_serial0 (int ch) {
    while (!(U0LSR & 0x20));
    return (U0THR = ch);
}

/* Write character to Serial Port 1 without \n -> \r\n  */
int putc_serial1 (int ch) {
    while (!(U1LSR & 0x20));
    return (U1THR = ch);
}

void putstring_serial0 (const char *string) {
    char ch;

    while ((ch = *string)) {
        putc_serial0(ch);
        string++;
    }
}

void putstring_serial1 (const char *string) {
    char ch;

    while ((ch = *string)) {
        putc_serial0(ch);
        string++;
    }
}


/* Read character from Serial Port, non-block   */
int getkey_serial0 (void) {
	if (U0LSR & 0x01) {
    return (U0RBR);
  } else {
    return 0;
  }
}

/* Read character from Serial Port   */
int getc0 (void) {
	while ( (U0LSR & 0x01) == 0 ); //Wait for character
	return U0RBR;
}
