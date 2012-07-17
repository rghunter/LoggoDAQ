/*
 * error.c
 *
 *  Created on: Jul 16, 2012
 *      Author: rghunter
 */


//private functions
#include "error.h"
#include "util.h"
#include "LPC214x.h"


void _error_set_light(int statnum, int onoff) {
  if(onoff){
    //on
    IOCLR0 = statnum;
  } else {
    // Off
    IOSET0 = statnum;
  }
}

//public functions

void error_red_flash(void) //Just endlesly loops flash until restart
{
	while(1){
		_error_set_light(STAT_RED,ON);
		util_delay_ms(200);
		_error_set_light(STAT_RED,OFF);
		util_delay_ms(200);
	}
}
void error_green_flash(void) //Just endlesly loops flash until restart
{
	while(1){
		_error_set_light(STAT_GREEN,ON);
		util_delay_ms(200);
		_error_set_light(STAT_GREEN,OFF);
		util_delay_ms(200);
	}
}
