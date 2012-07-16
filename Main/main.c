/*********************************************************************************
 * Logomatic Version Kwan Firmware
 * Modified beyond recognition from original 2008 Sparkfun firmware
 * Kwan Systems 2009
 * ******************************************************************************/

/*******************************************************
 *          Header Files
 ******************************************************/
#include <stdio.h>
#include <string.h>
#include "LPC214x.h"

//Main application subfiles
#include "setup.h"
#include "loop.h"
#include "main.h"
#include "conparse.h"

/*******************************************************
 *            MAIN
 * Arduino-style structure here
 ******************************************************/

int main (void) {
  setup();
  for(;;) loop();
}

/*******************************************************
 *          Utility functions
 ******************************************************/
 
static int light_mask[3]={0x00000004,0x00000800,0x80000000};

void set_light(int statnum, int onoff) {
  if(onoff & !powerSave){ 
    //on 
    IOCLR0 = light_mask[statnum]; 
  } else { 
    // Off
    IOSET0 = light_mask[statnum]; 
  } 
}

void blinklock(int maintainWatchdog, int blinkcode) {
  if(blinkcode==0) {
    for(;;) {
      set_light(0,ON);
      delay_ms(50);
      set_light(0,OFF);
      set_light(1,ON);
      delay_ms(50);
      set_light(1,OFF);
    }
  } else {
    for(;;) {
      for(int i=0;i<blinkcode;i++) {
        set_light(0,ON);
        delay_ms(250);
        set_light(0,OFF);
        delay_ms(250);
      }
      set_light(1,ON);
      delay_ms(250);
      set_light(1,OFF);
      delay_ms(250);
    }
  }
}

void delay_ms(int count) {
  int i;
  count *= 10000;
  for(i = 0; i < count; i++) asm volatile ("nop");
}
