/*
 * util.c
 *
 *  Created on: Jul 16, 2012
 *      Author: rghunter
 */

#include "util.h"
#include "setup.h"

void util_init(void)
{
	//Not really fleshed out at the moment
	return;

}
void util_delay_ms(int time_ms)
{
	int i;
	time_ms *= 10000;
	for(i = 0; i < time_ms; i++) asm volatile ("nop");
}

