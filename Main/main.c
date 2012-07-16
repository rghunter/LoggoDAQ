/*********************************************************************************
 * Logomatic V2 Firmware
 * Sparkfun Electronics 2008
 * ******************************************************************************/

/*******************************************************
 * 		     Header Files
 ******************************************************/
#include <stdio.h>
#include <string.h>
#include "LPC21xx.h"

//Data Streaming

//UART0 Debugging
#include "serial.h"
#include "rprintf.h"

//Needed for main function calls
#include "main_msc.h"
#include "fat16.h"
#include "armVIC.h"
#include "itoa.h"
#include "rootdir.h"
#include "sd_raw.h"


/*******************************************************
 * 		     Global Variables
 ******************************************************/

#define ON	1
#define OFF	0


/*******************************************************
 * 		 Function Declarations
 ******************************************************/

void Initialize(void);

void delay_ms(int count);


/*******************************************************
 * 		     	MAIN
 ******************************************************/

int main (void)
{
	int i;
	Initialize();
	// Flash Status Lights
	while(1)
	{
		for(i = 0; i < 5; i++)
		{
			stat(0,ON);
			delay_ms(50);
			stat(0,OFF);
			stat(1,ON);
			delay_ms(50);
			stat(1,OFF);
		}
	}
	
    	return 0;
}


/*******************************************************
 * 		     Initialize
 ******************************************************/

#define PLOCK 0x400

void Initialize(void)
{
	rprintf_devopen(putc_serial0);
	
	PINSEL0 = 0xCF351505;
	PINSEL1 = 0x15441801;
	IODIR0 |= 0x00000884;
	IOSET0 = 0x00000080;

	S0SPCR = 0x08;  // SPI clk to be pclk/8
	S0SPCR = 0x30;  // master, msb, first clk edge, active high, no ints

}
void stat(int statnum, int onoff)
{
	if(statnum) // Stat 1
	{
		if(onoff){ IOCLR0 = 0x00000800; } // On
		else { IOSET0 = 0x00000800; } // Off
	}
	else // Stat 0 
	{
		if(onoff){ IOCLR0 = 0x00000004; } // On
		else { IOSET0 = 0x00000004; } // Off
	}
}
void delay_ms(int count)
{
	int i;
	count *= 10000;
	for(i = 0; i < count; i++)
		asm volatile ("nop");
}

