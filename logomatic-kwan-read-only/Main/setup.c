#include "LPC214x.h"
#include "sd_raw.h"
#include "rootdir.h"
#include "setup.h"
#include "conparse.h"
#ifdef UART_SYS
#include "uart.h"
#endif
#include "sdbuf.h"
#ifdef ADC_SYS
#include "adc.h"
#endif
#include "main.h"
#include "command.h"
#include "sirfwrite.h"
#include "nmeawrite.h"
#include "debug.h"
#include "loop.h"
#include "sdbuf.h"

static char fn[]="LOK1_XXX.bin";
unsigned int CCLK;
unsigned int PCLK;

static void setClock(int y, int m, int d, int h, int n, int s) {
  YEAR=y;
  MONTH=m;
  DOM=d;
  DOW=0;
  DOY=0;
  HOUR=h;
  MIN=n;
  SEC=s;
}

static void setupClock(void) {
  //Turn off the clock
  CCR=0;
  //Set the PCLK prescaler and set the clock to use it, so as to run in sync with everything else.
  PREINT=PCLK/32768-1;
  PREFRAC=PCLK-((PREINT+1)*32768);
  
  //If the clock year is reasonable, it must have been set by 
  //some process before, so we'll leave it running.
  //If it is year 0, then it is runtime from last reset, 
  //so we should reset it.
  //If it is unreasonable, this is the first time around,
  //and we set it to zero to count runtime from reset
  if(YEAR<2000 || YEAR>2100) {
    CCR|=(1<<1);
    setClock(0,0,0,0,0,0);
    //Pull the subsecond counter out of reset
    CCR&=~(1<<1);
  }
  //Turn the clock on
  CCR|=(1<<0);
}

static void setupTimer1(void) {
  T1TCR=0x02;    //Turn off timer and reset counter   
  T1CCR=(1<<6);  //Turn on capture 1.2 on rising (6) edge
  T1MCR=0x02;    //Reset timer on match
  T1MR0=PCLK;    //Set the reset time to 1 second
  T1TCR=0x01;    //Out of reset and turn timer on
}

static void setupPins(void) {
  //  C    F    3    5    1    5    0    5
  // 1100 1111 0011 0101 0001 0101 0000 0101
  //  F E  D C  B A  9 8  7 6  5 4  3 2  1 0 
  // Pin 0.00 (TXO0)     - 01 - TxD0
  // Pin 0.01 (RXI0)     - 01 - RxD0
  // Pin 0.02 (STAT0)    - 00 - GPIO 0.02
  // Pin 0.03 (STOP)     - 00 - GPIO 0.03
  // Pin 0.04 (Card SCK) - 01 - SCK0
  // Pin 0.05 (Card DO)  - 01 - MISO0
  // Pin 0.06 (Card DI)  - 01 - MOSI0
  // Pin 0.07 (Card CS)  - 00 - GPIO 0.07
  // Pin 0.08 (TXO1)     - 01 - TxD1
  // Pin 0.09 (RXI1)     - 01 - RxD1
  // Pin 0.10 (7)        - 11 - AD1.2
  // Pin 0.11 (STAT1)    - 00 - GPIO 0.11
  // Pin 0.12 (8)        - 11 - AD1.3
  // Pin 0.13 (BATLV)    - 11 - AD1.4
  // Pin 0.14 (BSL)      - 00 - GPIO 0.14
  // Pin 0.15 (NC)       - 11 - AD1.5
  PINSEL0 = 0xCF351505;

  //  1    5    4    4    1    B    0    4
  // 0001 0101 0100 0100 0001 1011 0000 0100
  //  F E  D C  B A  9 8  7 6  5 4  3 2  1 0 
  // Pin 0.16 0 (NC)   - 00 - GPIO 0.16
  // Pin 0.17 1 (SCK)  - 01 - CAP1.2    (GPS PPS)
  // Pin 0.18 2 (MISO) - 00 - GPIO 0.18
  // Pin 0.19 3 (MOSI) - 00 - GPIO 0.19
  // Pin 0.20 4 (CS)   - 11 - EINT3
  // Pin 0.21 5 (6)    - 10 - AD1.6
  // Pin 0.22 6 (5)    - 01 - AD1.7
  // Pin 0.23 7 (Vbus) - 00 - GPIO 0.23
  // Pin 0.24 8 (none) - 00 - Reserved 
  // Pin 0.25 9 (4)    - 01 - AD0.4
  // Pin 0.26 A (D+)   - 00 - Reserved (USB)
  // Pin 0.27 B (D-)   - 01 - Reserved (USB)
  // Pin 0.28 C (3)    - 01 - AD0.1
  // Pin 0.29 D (2)    - 01 - AD0.2
  // Pin 0.30 E (1)    - 01 - AD0.3
  // Pin 0.31 F (LED3) - 00 - GPO only
  PINSEL1 = 0x15441804;

  // Pin 0.2,0.7,0.11 set to out
  IODIR0 =(1<<2) | (1<<7) | (1<<11);
  // Pin 0.7 set to high
  IOSET0 = (1<<7);

  S0SPCR = 0x08;  // SPI clk to be pclk/8
  S0SPCR = 0x30;  // master, msb, first clk edge, active high, no ints
}

static int fat_initialize(void) {
  int result=sd_raw_init();
  if(result<0) return result;
  return openroot();
}

static void measurePCLK(void) {
  CCLK=FOSC*((PLLSTAT & 0x1F)+1);
  switch (VPBDIV & 0x03) {
    case 0:
      PCLK=CCLK/4;
      break;
    case 1:
      PCLK=CCLK;
      break;
    case 2:
      PCLK=CCLK/2;
      break;
    case 3:
      break;
  }
}

static void writeMAMsetup(void) {
  switch(adcMode) {
    case ADC_SIRF:
      fillStartSirf(&sdBuf,0x17);
      fill(&sdBuf,MAMCR);
      fill(&sdBuf,MAMTIM);
      fillShort(&sdBuf,PLLSTAT);
      fill(&sdBuf,VPBDIV);
      fillInt(&sdBuf,CCLK);
      fillInt(&sdBuf,PCLK);
      fillFinishSirf(&sdBuf);
      break;
    case ADC_TEXT: 
      fillString(&sdBuf,"MAM setup: MAMCR=");
      fillDec(&sdBuf,MAMCR);
      fillString(&sdBuf,", MAMTIM=");
      fillDec(&sdBuf,MAMTIM);
      fillShort(&sdBuf,0x0D0A);
      mark(&sdBuf);
      break;
    case ADC_BINARY:
      break;
    case ADC_NMEA: 
      fillStartNMEA(&sdBuf,'M');
      fillDec(&sdBuf,MAMCR);
      fill(&sdBuf,',');
      fillDec(&sdBuf,MAMTIM);
      fill(&sdBuf,',');
      fillDec(&sdBuf,((PLLSTAT >>  0) & 0x1F)+1); //MSEL, PLL Multiplier
      fill(&sdBuf,',');
      fillDec(&sdBuf,(PLLSTAT >>  5) & 0x03);     //PSEL, PLL Divisor
      fill(&sdBuf,',');
      fillDec(&sdBuf,(PLLSTAT >>  8) & 0x01);     //PLLE, PLL Enabled
      fill(&sdBuf,',');
      fillDec(&sdBuf,(PLLSTAT >>  9) & 0x01);     //PLLC, PLL Connected
      fill(&sdBuf,',');
      fillDec(&sdBuf,(PLLSTAT >> 10) & 0x01);     //PLOCK, Phase Lock
      fill(&sdBuf,',');
      fillDec(&sdBuf,VPBDIV);                     //VPB Divisor
      fill(&sdBuf,',');
      fillDec(&sdBuf,CCLK);                       //Peripheral Clock rate, Hz
      fill(&sdBuf,',');
      fillDec(&sdBuf,PCLK);                       //Peripheral Clock rate, Hz
      fill(&sdBuf,',');
      fillDec(&sdBuf,PREINT);                     //Peripheral Clock rate, Hz
      fill(&sdBuf,',');
      fillDec(&sdBuf,PREFRAC);                    //Peripheral Clock rate, Hz
      fill(&sdBuf,',');
      fillHex(&sdBuf,CCR,2);                    //Peripheral Clock rate, Hz
      fillFinishNMEA(&sdBuf);
      break;
  }
}

char versionString[]="Logomatic Kwan v1.1 "__DATE__" "__TIME__;

static void writeVersion(void) {
  switch(adcMode) {
    case ADC_SIRF:
      fillStartSirf(&sdBuf,0x1A);
  	  fillString(&sdBuf,versionString);
      fillFinishSirf(&sdBuf);
      break;
    case ADC_TEXT: 
      fillString(&sdBuf,versionString);
      fillShort(&sdBuf,0x0D0A);
      mark(&sdBuf);
      break;
    case ADC_BINARY:
      break;
    case ADC_NMEA: 
      fillStartNMEA(&sdBuf,'V');
  	  fillString(&sdBuf,versionString);
      fillFinishNMEA(&sdBuf);
      break;
  }
}

static void writeSDinfo(void) {
  char buf[36];
  char result=sd_raw_get_cid_csd(buf);
  
  switch(adcMode) {
    case ADC_SIRF:
      fillStartSirf(&sdBuf,0x18);
      fill(&sdBuf,result);
      for(int i=0;i<36;i++) fill(&sdBuf,buf[i]);
  
      fillFinishSirf(&sdBuf);
      break;
    case ADC_TEXT: 
      fillString(&sdBuf,"SD Card Setup: result=");
      fillDec(&sdBuf,result);
      fillShort(&sdBuf,0x0D0A);
      int pos=0;
      fillString(&sdBuf,"  MID=");fillDec(&sdBuf,buf[pos]);pos++;fillShort(&sdBuf,0x0D0A);
      fillString(&sdBuf,"  OID='");
      for(int i=0;i<2;i++) {
        fill(&sdBuf,buf[pos]);pos++;
      }
      fillString(&sdBuf,"'\r\n");
      fillString(&sdBuf,"  PNM='");
      for(int i=0;i<5;i++) {
        fill(&sdBuf,buf[pos]);pos++;
      }
      fillString(&sdBuf,"'\r\n");
      fillString(&sdBuf,"  PRV=");fillDec(&sdBuf,buf[pos]);pos++;fillShort(&sdBuf,0x0D0A);
      unsigned int PSN=0;
      for(int i=0;i<4;i++) {
        PSN=(PSN<<8)+buf[pos];pos++;
      }
      fillString(&sdBuf,"  PSN=");fillDec(&sdBuf,PSN);fillShort(&sdBuf,0x0D0A);
      mark(&sdBuf);
      break;
    case ADC_BINARY:
      break;
    case ADC_NMEA: 
      break;
  }
}




void setup() {
  
  int i;
  int count = 0;
  
  measurePCLK();
  
  setupTimer1();

  setupClock();
  
  setupPins();
  set_light(2,OFF);
  
  if(fat_initialize()<0) blinklock(0,1);

  // Flash Status Lights

  for(i = 0; i < 5; i++) {
    set_light(0,ON);
    delay_ms(50);
    set_light(0,OFF);
    set_light(1,ON);
    delay_ms(50);
    set_light(1,OFF);
  }

  count=0;
  do {
    if(count >=99) blinklock(0,0);
    fn[5]=count/100+'0';
    fn[6]=(count%100)/10+'0';
    fn[7]=(count%10)+'0';
    count++;
  } while(root_file_exists(fn));
  
  if(open_debug()<0) blinklock(0,4);
  if(openSD(fn)<0) blinklock(0,2);
  debug_print("After openSD() - ouf.dir_entry\n");
  debug_printDE("",&ouf.dir_entry);debug_flush();
  
  if(readLogCon()<0) blinklock(0,3);
  
  if(hasStartTime) setClock(startY,startM,startD,startH,startN,startS);
  
  debug_print("After readLogCon() - ouf.dir_entry\n");
  debug_printDE("",&ouf.dir_entry);debug_flush();

  if(loadCommands()<0) blinklock(0,4);

  writeVersion();

  writeADCsetup();
  writeMAMsetup();
  writeSDinfo();
  

  startRecordUART();

  //Only set up the ADC if some channels are to be recorded  
  if(nChannels>0 && adcFreq>0) startRecordADC(); 
  debug_print("Finished setup()\n");debug_flush();
}

