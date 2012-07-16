#include "LPC214x.h"
#include "adc.h"
#include "conparse.h"
#include "armVIC.h"
#include "load.h"
#include "circular.h"
#include "sirfwrite.h"
#include "nmeawrite.h"
#include "setup.h"
#include "main.h"

#ifdef ADC_SYS

//Set sample clock to near 4.5MHz (Divisor 14). Much faster and
//marginally better noise than Divisor 255
#define ADCDIV 14

circular adcBuf;
static int binNum;
static unsigned int adcReading[9];
static unsigned int adcReadingCopy[9];
static unsigned int tsc1Copy;

static void writeADC(void) {
  switch(adcMode) {
    case ADC_TEXT: 
      fillDec(&adcBuf,tsc1Copy/60000);
      for(int i=0;i<nChannels;i++) {
        if(channelActive[i]) {
          fill(&adcBuf,' ');
          fillDec(&adcBuf,adcReadingCopy[i]);
        }
      }
      fillShort(&adcBuf,0x0D0A); 
      mark(&adcBuf);
      break;
    case ADC_BINARY:
      for(int i=0;i<nChannels;i++) {
        if(channelActive[i]) {
          fillShort(&adcBuf,adcReadingCopy[i]);
        }
      }
      fillShort(&adcBuf,0x2424);
      mark(&adcBuf);
      break;
    case ADC_NMEA: 
      fillStartNMEA(&adcBuf,'A');
      fillDec(&adcBuf,tsc1Copy/60000);
      for(int i=0;i<nChannels;i++) {
        if(channelActive[i]) {
          fill(&adcBuf,',');  
          fillDec(&adcBuf,adcReadingCopy[i]);
        }
      }
      fillFinishNMEA(&adcBuf);
      break;
    case ADC_SIRF:
      fillStartSirf(&adcBuf,0x2C);
      for(int i=0;i<=8;i++) {
        if(channelActive[i]) {
          fillShort(&adcBuf,adcReadingCopy[i] & 0xFFFF);
        }
      }
      fillInt(&adcBuf,tsc1Copy);
      fillFinishSirf(&adcBuf);
      break;
  }
}

//Internal names for each ADC pin (AD0 is internal battery monitor)
//             0 1 2 3 4 5 6 7 8
int adcSide[]={1,0,0,0,0,1,1,1,1};
int adcChan[]={4,3,2,1,4,7,6,2,3};

int adcSequence[5][2]={{3,4},{2,7},{1,6},{4,2},{0,3}};
int adcMask[5+1]={3,3,3,3,2,0};
int adcBackref[5][2]={{1,0},{2,5},{3,6},{4,7},{-1,8}};

volatile static int inAdcIsr=0;

static void adcISR(void) {
  if(inAdcIsr) return;
  inAdcIsr=1;
  T0IR = 1; // Clear T0 interrupt on match channel 0

  int temp=0;
  int j;
  hasLoad(LOAD_ADC);
  
  for(j=0;adcMask[j]>0;j++) {
    AD0CR=(1<<21) | ((ADCDIV-1)<<8) | (1 << adcSequence[j][0]); //Turn on AD0, program the divisor, set the input channel
    AD1CR=(1<<21) | ((ADCDIV-1)<<8) | (1 << adcSequence[j][1]); //Turn on AD1, program the divisor, set the input channel
    switch(adcMask[j]) {
      case 1:
        AD0CR|=0x01000000; //Start conversion just on AD0
        break;
      case 2:
        AD1CR|=0x01000000; //Start conversion just on AD1
        break;
      case 3:
        ADGSR|=0x01000000; //Start conversion on both
    }
    do {
      switch(adcMask[j]) {
        case 1:
          temp=AD0GDR; //Check if channel 0 is done
          break;
        case 2:
        case 3:
          temp=AD1GDR; //Check if channel 1 is done
          break;
      }
    } while((temp & 0x80000000) == 0);
    if(adcMask[j] & 0x01) {
      AD0CR=0x00000000; //Stop AD0
      adcReading[adcBackref[j][0]]+=(AD0GDR & 0xFFC0) >> 6;
    }
    if(adcMask[j] & 0x02) {
      AD1CR=0x00000000; //Stop AD1
      adcReading[adcBackref[j][1]]+=(AD1GDR & 0xFFC0) >> 6;
    }
  }

  unsigned int tsc1 = T1TC;

  binNum++; 
  if(binNum==adcBin) {
    tsc1Copy=tsc1;
    for(j=0;j<9;j++) {
      adcReadingCopy[j]=adcReading[j];
      adcReading[j]=0;
    }
    binNum=0;
    writeADC();
  }
  inAdcIsr=0;
  VICVectAddr= 0;
}

void writeADCsetup() {
  switch(adcMode) {
    case ADC_NMEA:
      fillStartNMEA(&adcBuf,'C');
      fillDec(&adcBuf,adcFreq);
      fill(&adcBuf,',');  
      fillDec(&adcBuf,adcBin);
      fill(&adcBuf,',');  
      fillDec(&adcBuf,ADCDIV);

      for(int i=0;i<nChannels;i++) {
        if(channelActive[i]) {
          fill(&adcBuf,',');  
          fillDec(&adcBuf,i);
          fill(&adcBuf,',');
          fillDec(&adcBuf,adcSide[i]);
          fill(&adcBuf,'.');
          fillDec(&adcBuf,adcChan[i]);
        }
      }

      fillFinishNMEA(&adcBuf);
      break;
    case ADC_SIRF:
      fillStartSirf(&adcBuf,0x2A);
      fillShort(&adcBuf,adcFreq);
      fillShort(&adcBuf,adcBin);
      fill(&adcBuf,ADCDIV);
      for(int i=0;i<nChannels;i++) {
        if(channelActive[i]) {
          fill(&adcBuf,i);
  	      fill(&adcBuf,adcSide[i]<<4 | adcChan[i]);
        }
      } 
      fillFinishSirf(&adcBuf);
      break;
    default:
      break;
  }
}

void startRecordADC(void) {
  enableIRQ();
  // Timer0  interrupt is an IRQ interrupt
  VICIntSelect &= ~0x00000010;
  // Enable Timer0 interrupt
  VICIntEnable |= 0x00000010;
  // Use slot 3 for Timer0 interrupt
  VICVectCntl3 = 0x24;
  // Set the address of ISR for slot 3
  VICVectAddr3 = (unsigned int)adcISR;

  T0TCR = 0x00000002;  // Reset counter and prescaler and disable timer
  T0CTCR= 0x00000000;  // Timer 0 is to be used as a timer, not a counter
  T0MCR = 0x00000003;  // On match 0 reset the counter and generate interrupt. All other match channels ignored
  T0MR0 = PCLK / adcFreq;  //Match channel 0 value: When T0TC reaches this value, things happen as described above

  T0PR = 0x00000000;   //No prescale - 1 PCLK tick equals 1 Timer0 tick
  T0CCR= 0x00000000;   //No capture on external input for Timer0
  T0EMR= 0x00000000;   //No external output on matches for Timer0

  T0TCR = 0x00000001; // enable timer
}

#endif
