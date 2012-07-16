#include "LPC214x.h"
#include "loop.h"
#include "adc.h"
#include "setup.h"
#include "uart.h"
#include "main.h"
#include "sdBuf.h"
#include "command.h"
#include "load.h"
#include "sirfwrite.h"
#include "nmeawrite.h"
#include "gps.h"
#include "conparse.h"
#include "debug.h"
#include "sd_raw.h"

static unsigned int lastcap;
int readyForOncePerSecond=0;

static void fillTime(circular* buf) {
  fill0Dec(buf,YEAR,4);
  fill0Dec(buf,MONTH,2);
  fill0Dec(buf,DOM,2);
  fill(buf,',');
  fill0Dec(buf,HOUR,2);
  fill0Dec(buf,MIN,2);
  fill0Dec(buf,SEC,2);
}

static PPSLight=0;

static void checkPPS(void) {
  unsigned int thiscap=T1CR2 % 60000000;
  if(lastcap!=thiscap) {
    hasLoad(LOAD_LOAD);
    lastcap=thiscap;
    int USEC=lastcap/60;
    PPSLight=1-PPSLight;
    switch(adcMode) {
      case ADC_TEXT:
        fillString(&sdBuf, "PPS: ");
   	    fillTime(&sdBuf);
        fill(&sdBuf,'.');
        fill0Dec(&sdBuf,USEC,6);
        fillShort(&sdBuf,0x0D0A);
        mark(&sdBuf);
      case ADC_NMEA:
        fillStartNMEA(&sdBuf,'P');
        fillTime(&sdBuf);
        fill(&sdBuf,'.');
        fill0Dec(&sdBuf,USEC,6);
        fillFinishNMEA(&sdBuf);
        break;
      case ADC_SIRF:
        fillStartSirf(&sdBuf,0x19);
        fillShort(&sdBuf,YEAR);
        fill(&sdBuf,MONTH);
        fill(&sdBuf,DOM);
        fill(&sdBuf,HOUR);
        fill(&sdBuf,MIN);
        fill(&sdBuf,SEC);
        fillInt(&sdBuf,lastcap);
        fillFinishNMEA(&sdBuf);
        break;
    }
  }
}

static void oncePerSecond(void) {
  writeCommand();
}

void loop(void) {
  checkPPS();

  if(readylen(&adcBuf)>0) {
    hasLoad(LOAD_ADC);
    if(isFlushSDNeeded()) {
      hasLoad(LOAD_FLUSH);
      flushSD();
    }
    drain(&adcBuf,&sdBuf);
  }

  for(int i=0;i<2;i++) if(readylen(&uartbuf[i])>0) {
    hasLoad(LOAD_UART);
    if(isFlushSDNeeded()) {
      hasLoad(LOAD_FLUSH);
      flushSD();
    }
    if(timestamp[i]) {
      fillTime(&sdBuf);
      fill(&sdBuf,' ');
      fillDec(&sdBuf,i);
      fill(&sdBuf,' ');
    }
    drain(&uartbuf[i],&sdBuf);
  }
  if(isFlushSDNeeded()) {
    hasLoad(LOAD_FLUSH); 
    flushSD();
  }

  if(readyForOncePerSecond) {
    readyForOncePerSecond=0;
    oncePerSecond();
  }
  // if button pushed, log file & quit
  if((IOPIN0 & 0x00000008) == 0) {
    VICIntEnClr = 0xFFFFFFFF; //Turn off interrupts
    //Write out the last buffers
    mark(&adcBuf);
    drain(&adcBuf,&sdBuf);
    flushSD();
    for(int i=0;i<2;i++) {
      mark(&uartbuf[i]);
      drain(&uartbuf[i],&sdBuf);
      flushSD();
    }
    flushSDLast();
    sd_raw_sync();
    //quit
    blinklock(1,0);
  }
  set_light(0,GPSLight);

  countLoad();
  clearLoad();
  if(powerSave) sleep();
}

