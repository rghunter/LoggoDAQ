#include <string.h>
#include "LPC214x.h"
#include "load.h"
#include "loop.h"
#include "sirfwrite.h"
#include "nmeawrite.h"
#include "main.h"
#include "conparse.h"
#include "sdbuf.h"
#include "setup.h"
#include "stringex.h"
#include "loop.h"

static int lasttoc=-1;
static int load[16];
static int lastIdle;
volatile static int loadMask;

static void writeLoad(void) {
  switch(adcMode) {
    case ADC_NMEA:
      fillStartNMEA(&sdBuf,'L');
      fillDec(&sdBuf,YEAR);
      fill(&sdBuf,',');
      fillDec(&sdBuf,MONTH);
      fill(&sdBuf,',');
      fillDec(&sdBuf,DOM);
      fill(&sdBuf,',');
      fillDec(&sdBuf,HOUR);
      fill(&sdBuf,',');
      fillDec(&sdBuf,MIN);
      fill(&sdBuf,',');
      fillDec(&sdBuf,SEC);
      for(int i=0;i<16;i++) {
        fill(&sdBuf,',');
        fillDec(&sdBuf,load[i]);
      }
      fillFinishNMEA(&sdBuf);
      break;
    case ADC_SIRF:
      fillStartSirf(&sdBuf,0x15);
      fillShort(&sdBuf,YEAR);
      fill(&sdBuf,MONTH);
      fill(&sdBuf,DOM);
      fill(&sdBuf,HOUR);
      fill(&sdBuf,MIN);
      fill(&sdBuf,SEC);
      fillShort(&sdBuf,CTC);
      for(int i=0;i<16;i++) {
        fillInt(&sdBuf,load[i]);
      }
      fillFinishSirf(&sdBuf);
      break;
  }
}

int countLoad() {
  int result=0;

  int thistoc=T1TC;
  //Handle the last little bit of each timer cycle
  if(lasttoc>0) {
    if(thistoc<lasttoc) {
      //Get the last little bit of time around the corner
      hasLoad(LOAD_LOAD);
      load[loadMask]+=PCLK-lasttoc;
      writeLoad();
      readyForOncePerSecond=1;
	    result=1;
      //Reset the counters
      lastIdle=load[0];
      for(int i=0;i<16;i++) load[i]=0;
      //Count the rest of the time in this tick
      load[loadMask]=thistoc;
    } else {
      load[loadMask]+=(thistoc-lasttoc);
    }
  }
  lasttoc=thistoc;
  return result;
}

void hasLoad(char task) {
  loadMask|=(1<<task);
  set_light(1,ON);
}

void clearLoad() {
  loadMask=0;
  set_light(1,OFF);
}

void sleep() {
  PCON|=1; //idle mode
}
