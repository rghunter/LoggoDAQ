#include "LPC214x.h"
#include "armVIC.h"
#include "uart.h"
#include "main.h"
#include "conparse.h"
#include "setup.h"
#include "load.h"
#include "serial.h"
#include "gps.h"
#include "setup.h"
#include "nmeawrite.h"
#include "sirfwrite.h"
#include "debug.h"

#ifdef UART_SYS

circular uartbuf[2];
volatile int uartInPkt[2];

#define STANDARD_BAUD_NUM 9
static int standardBaud[STANDARD_BAUD_NUM]={1200,2400,4800,9600,14400,19200,38400,57600,115200};
             
inline static int isFrameStart(int port, char next) {
  if(UART_NMEA==uartMode[port]) {
    return (next==trigger[port]);
  } else if(UART_SIRF==uartMode[port]) {
    return (next==0xA0);
  }
  return 1;
}

inline static int isFrameEndNMEA(int port) {
  return (peekHead(&uartbuf[port],-1)==frameEnd[port]);
}

inline static int isFrameEndSiRF(int port) {
  return (peekHead(&uartbuf[port],-1)==0xB3 && peekHead(&uartbuf[port],-2)==0xB0);
}
 
inline static int isFrameEnd(int port) {
  if(UART_NMEA==uartMode[port] || UART_TEXT==uartMode[port]) {
    return isFrameEndNMEA(port);  
  } else if(UART_SIRF==uartMode[port]) {
    return isFrameEndSiRF(port);
  }
  return unreadylen(&uartbuf[port])<rawSize[port];
}

static void UARTISR(int port) {
  char next,status;
  
  //Read the whole FIFO
  hasLoad(LOAD_UART);
  status=ULSR(port);
  while(status & 0x01) {
    next=URBR(port);
    if(uartInPkt[port] || isFrameStart(port,next)) { //If already in the frame, or this character starts it
      uartInPkt[port]=1; //We're still in the frame
      fill(&uartbuf[port],next);
      if(isFrameEnd(port) || unreadylen(&uartbuf[port])>=912) {
        //But not after this character
        uartInPkt[port]=0;
        if(GPSSyncMode>0) { //Only parse the packet if we need to
          if(uartMode[port]==UART_SIRF) {
            parseSirf(&uartbuf[port]);
          } else if(uartMode[port]==UART_NMEA) {
            parseNmea(&uartbuf[port]);
          }
        }
        mark(&uartbuf[port]);
      }
    }
    status=ULSR(port);
  }

  next = UIIR(port); // Have to read this to clear the interrupt 
  VICVectAddr = 0;
}

static void UART0ISR(void) {
  UARTISR(0);
}

static void UART1ISR(void) {
  UARTISR(1);
}


void setup_uart(int port, int setbaud, int want_ints) {
  ULCR(port) = 0x83;   // 8 bits, no parity, 1 stop bit, DLAB = 1
  //DLAB - Divisor Latch Access bit. When set, a certain memory address
  //       maps to the divisor latches, which control the baud rate. When
  //       cleared, those same addresses correspond to the processor end 
  //       of the FIFOs. In other words, set the DLAB to change the baud
  //       rate, and clear it to use the FIFOs.

  int Denom=PCLK/setbaud;
  int UDL=Denom/16;
  
  UDLM(port)=(UDL >> 8) & 0xFF;
  UDLL(port)=(UDL >> 0) & 0xFF;
  
  UFCR(port) = 0xC1; //Enable both FIFOs
  ULCR(port) = 0x03; //Turn of DLAB - FIFOs accessable
  
  if(want_ints == 1) {
    enableIRQ();
    VICIntSelect &= ~(0x00000040<<port);  //Make this a normal interrupt instead of FIQ
    VICIntEnable |= (0x00000040<<port);   //Enable the interrupt
	if(port==0) {
      VICVectCntl1 = 0x26;          //VIC slot 1 enabled, IRQ6 (UART0)
      VICVectAddr1 = (unsigned int)UART0ISR;
	  U0IER=1;
	} else {
      VICVectCntl2 = 0x27;          //VIC slot 2 enabled, IRQ7 (UART1)
      VICVectAddr2 = (unsigned int)UART1ISR;
	  U1IER=1;
    }	
  } else {
    VICIntEnClr = 0x00000040<<port;
    UIER(port) = 0x00;
  }

  switch(adcMode) {
    case ADC_TEXT:
      fillString(&uartbuf[port],"UART Setup: ");
      fillDec(&uartbuf[port],port);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],setbaud);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],Denom);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],UDL);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],UFDR(port));
      fillShort(&uartbuf[port],0x0D0A);
      mark(&uartbuf[port]);
      break;
    case ADC_NMEA:
      fillStartNMEA(&uartbuf[port],'U');
      fillDec(&uartbuf[port],port);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],setbaud);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],Denom);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],UDL);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],UFDR(port));
      fillFinishNMEA(&uartbuf[port]);
      break;
    case ADC_SIRF:
      fillStartSirf(&uartbuf[port],0x2B);
      fill(&uartbuf[port],port & 0xFF);
      fillInt(&uartbuf[port],setbaud);
      fillShort(&uartbuf[port],UDL);
      fill(&uartbuf[port],UFDR(port));
      fillFinishSirf(&uartbuf[port]);
      break;
  }
}

void startRecordUART(void) {
  if (uartMode[0]!=UART_NONE) {
    if(baud[0]<0) autobaud(0);
    if(baud[0]>0) setup_uart(0,baud[0],1);
  }
  if (uartMode[1]!=UART_NONE) {
    if(baud[1]<0) autobaud(1);
    if(baud[1]>0) setup_uart(1,baud[1],1);
  }
}

#ifdef COMMAND_SYS
void sendBuf(int port, circular* buf) {
  int len=unreadylen(buf);
  if(uartMode[port]!=UART_NONE) {
    if(port==0) {
      for(int i=0;i<len;i++) putc_serial0(peekMid(buf,i));
    } else {
      for(int i=0;i<len;i++) putc_serial1(peekMid(buf,i));
    }
  }
  mark(buf);
}    
#endif

//Returns positive if it looks like this is a good baud
//Returns negative otherwise
//Good baud has at least 20 characters in a second
//and less than 1/100 of characters have an error
static int countCharBaud(int port, int trybaud, unsigned int* chars, unsigned int* errors) {
  *chars=0;
  *errors=0;
  setup_uart(port,trybaud,0);
  
  unsigned int start=T1TC;
  while((T1TC-start)<60000000) {
    int status=ULSR(port);
    if(status & 0x01) {
      char junk=URBR(port);
      (*chars)++;
      if(status & 0x80) (*errors)++;
    }
  }
  int result;
  if (*chars<20) {
    result=-1;
  } else if (*errors>(*chars/100)) {
    result=-2;
  } else {
    result=1;
  }
  switch(adcMode) {
    case ADC_NMEA:
      fillStartNMEA(&uartbuf[port],'B');
      fillDec(&uartbuf[port],port);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],trybaud);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],*chars);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],*errors);
      fill(&uartbuf[port],',');
      fillDec(&uartbuf[port],result);
      fillFinishNMEA(&uartbuf[port]);
      break;
    case ADC_SIRF:
      fillStartSirf(&uartbuf[port],0x16);
      fill(&uartbuf[port],port);
      fillInt(&uartbuf[port],trybaud);
      fillInt(&uartbuf[port],*chars);
      fillInt(&uartbuf[port],*errors);
      fillInt(&uartbuf[port],result);
      fillFinishSirf(&uartbuf[port]);
      break;
  }
  return result;
}

int autobaud(int port) {
  int lighton=0;
  int leastErrors=99999;
  int leastBaud=-1;
  unsigned int chars,errors;
  for(int i=0;i<STANDARD_BAUD_NUM;i++) {
    lighton=1-lighton;
    set_light(port,lighton);
    int result=countCharBaud(port,standardBaud[i],&chars,&errors);
    if(result>0) {
      baud[port]=standardBaud[i];
      set_light(port,OFF);
      return result;
    }
	if(result==-2) {
	  if(errors<(chars/10) && errors<leastErrors) {
	    leastErrors=errors;
		leastBaud=i;
      }
	}
  }
  if(leastBaud>=0) {
    baud[port]=standardBaud[leastBaud];
	set_light(port,OFF);
	return 2;
  }
  //Couldn't lock on, don't use the port
  uartMode[port]=UART_NONE;
  baud[port]=0;
  set_light(port,OFF);
  return -1;
}

#endif
