#include <stdio.h>
#include <string.h>
#include "conparse.h"
#include "stringex.h"
#include "fat16.h"
#include "rootdir.h"
#include "sd_raw.h"
#include "debug.h"
#include "setup.h"
#include "main.h"

//actual definitions of config parameters
int uartMode[2]={1,1};
int baud[2]={9600,9600};
char trigger[2]={'$','$'};
char frameEnd[2]={'\n','\n'};
int rawSize[2]={64,64};
char timestamp[2]={0,0};
int adcMode=1;
int adcFreq=100;
int adcBin=1;
char channelActive[9]={0,0,0,0,0,0,0,0,0};
int nChannels=0;
int hasStartTime;
int startY, startM, startD, startH, startN, startS;
int GPSSyncMode=0;
int powerSave=0;

//Log configuration tags and default values, used to write the LogCon.txt if not present
//and to read LogCon.txt.
//Specified as one long string here, but used as a ragged array of strings terminated by a double \0
static char LogConTags[]="UART0 mode\0"       //NMEA, Text, Raw, SiRF, off (anything else)
                         "UART0 baud\0"  //baud code
                         "UART0 trigger\0"    //Trigger, same as before (Only effective in NMEA)
                         "UART0 end\0"        //Frame end character (Only effective in NMEA)
                         "UART0 size\0"        
                         "UART0 timestamp\0"
                         "UART1 mode\0"       //NMEA, Text, Raw, SiRF, off (anything else)
                         "UART1 baud\0"  //baud code
                         "UART1 trigger\0"    //Trigger, same as before (Only effective in NMEA)
                         "UART1 end\0"        //Frame end character (Only effective in NMEA)
                         "UART1 size\0"        
						             "UART1 timestamp\0"
						             "Start Time\0"
						             "GPS Sync\0"
						             "Powersave\0"
                         "ADC mode\0"        //Text, Binary, SiRF, off (anything else)
                         "ADC frequency\0"   //ADC read rate, Hz
                         "ADC binning\0"     //ADC binning rate
                         "AD0\0"
                         "AD1\0"
                         "AD2\0"
                         "AD3\0"
                         "AD4\0"
                         "AD5\0"
                         "AD6\0"
                         "AD7\0"
                         "AD8\0\0";
static char LogConDefault[]="None\0"
                            "0\0"
                            "$\0"
                            "0x0A\0"
                            "64\0"
                            "N\0"
                            "None\0"
                            "0\0"
                            "$\0"
                            "0x0A\0"
                            "64\0"
                            "N\0"
                            "20090704000000\0"
                            "1\0"
                            "1\0"
                            "Text\0"
                            "100\0"
                            "1\0"
                            "Y\0"
                            "Y\0"
                            "Y\0"
                            "Y\0"
                            "Y\0"
                            "Y\0"
                            "Y\0"
                            "Y\0"
                            "Y\0\0";
        
static int processLine(char* keyword, char* value) {
  trim(keyword);
  trim(value);
  int whichKeyword=0;
  int whereInTags=0;
  char done=0;
  while(!done) {
    if(0==strcasecmp(keyword,&LogConTags[whereInTags])) {
      done=1;
    } else {
      whichKeyword++;
      while(LogConTags[whereInTags]!=0) whereInTags++;
      whereInTags++;
      if(LogConTags[whereInTags]==0) return 0; //Didn't find the keyword
    }
  }

  if(whichKeyword<12) {
    int port=whichKeyword/6;
    int word=whichKeyword%6;
    switch(word) {
      case  0: //"UART0 mode\0"       //NMEA, Hex, Raw, SiRF, off (anything else)
        if(0==strcasecmp(value,"NMEA")) {
          uartMode[port]=UART_NMEA;
        } else if(0==strcasecmp(value,"Text")) {
          uartMode[port]=UART_TEXT;
        } else if(0==strcasecmp(value,"Raw")) {
          uartMode[port]=UART_RAW;
        } else if(0==strcasecmp(value,"SiRF")) {
          uartMode[port]=UART_SIRF;
        } else if(0==strcasecmp(value,"Both")) {
          uartMode[port]=UART_BOTH;
        } else {
          uartMode[port]=UART_NONE;
        }
        break;
      case  1: //"UART0 baud\0"
        if(0==strcasecmp(value,"Auto")) {
          baud[port]=-1;
        } else {
          baud[port]=stoi(value);
        }
        break;
      case  2: //"UART0 trigger\0"    //Trigger, same as before (Only effective in NMEA)
        if(strlen(value)>1) {
          trigger[port]=stoi(value);
        } else {
          trigger[port]=value[0];
        }
        break;
      case  3: //"UART0 end\0"        //Frame end character (Only effective in NMEA)
        if(strlen(value)>1) {
          frameEnd[port]=stoi(value);
        } else {
          frameEnd[port]=value[0];
        }
        break;
      case  4: //"UART0 size\0"        
        rawSize[port]=stoi(value);
        break;
      case  5: //"UART0 Timestamp\0"        
        rawSize[port]=stoi(value);
        timestamp[port]=(value[0]=='Y' || value[0]=='y');
        break;
    }
  } else if(whichKeyword==12) {
    startS=stoi(&value[12]);
  	value[12]=0;
	  startN=stoi(&value[10]);
	  value[10]=0;
	  startH=stoi(&value[8]);
	  value[8]=0;
	  startD=stoi(&value[6]);
	  value[6]=0;
	  startM=stoi(&value[4]);
	  value[4]=0;
	  startY=stoi(value);
	  hasStartTime=1;
  } else if(whichKeyword==13) {
    GPSSyncMode=stoi(value);
  } else if(whichKeyword==14) {
    powerSave=stoi(value);
  } else if(whichKeyword==15) {
    if(0==strcasecmp(value,"NMEA")) {
      adcMode=ADC_NMEA;
    } else if(0==strcasecmp(value,"Binary")) {
      adcMode=ADC_BINARY;
    } else if(0==strcasecmp(value,"SiRF")) {
      adcMode=ADC_SIRF;
    } else if(0==strcasecmp(value,"Text")) {
      adcMode=ADC_TEXT;
    } else {
      adcMode=ADC_NONE;
    }
  } else if(whichKeyword==16) {
    adcFreq=stoi(value);
  } else if(whichKeyword==17) {
    adcBin=stoi(value);
  } else {
    int channelNum=whichKeyword-18;
    if(channelNum>8) return -1;
    channelActive[channelNum]=(value[0]=='Y' || value[0]=='y');
    nChannels+=channelActive[channelNum];
  }
  return 0;
}

//0 for success, negative for failure
int readLogCon(void) {
  char keyword[64];
  char value[64];                            
  char stringBuf[512];

  struct fat16_file_struct fd;
  int len;
  int s=0;
  int t=0;
  int d=0;
  hasStartTime=0;
  keyword[0]=0;
  value[0]=0;
  int result;
//  root_open_new(&fd,"FIRMDUMP.bin");
//  for(int i=0;i<1024;i++) fat16_write_file(&fd,512*i,512);
//  fat16_close_file(&fd);
//  sd_raw_sync();
  if(root_file_exists("LOGCON.txt")) {
    result = root_open(&fd,"LOGCON.txt");
    
    if(result<0) return result;
    len = fat16_read_file(&fd, (unsigned char *)stringBuf, 512);
    if(len<0) return -1;
    fat16_close_file(&fd);
  } else {
    result=root_open_new(&fd,"LOGCON.txt");
    if(result<0) return result;

    //I hate pointer math!    
    while(LogConTags[t]!=0) {
      while(LogConTags[t]!=0) {
        stringBuf[s]=LogConTags[t];
        s++;t++;
      }
      t++; //skip the \0 at the end of the tag
      stringBuf[s]='=';s++;
      while(LogConDefault[d]!=0) {
        stringBuf[s]=LogConDefault[d];
        s++;d++;
      }
      d++; //skip the \0 at the end of the default
      stringBuf[s]='\r';s++;
      stringBuf[s]='\n';s++;
    }
    len=s;
    fat16_write_file(&fd, (unsigned char*)stringBuf, len);
    sd_raw_sync();
  }
  
  s=0;
  t=0;
  char inValue=0;
  int lines=0;
  for(s = 0; s < len; s++) {
    if(stringBuf[s] == '=') {
      keyword[t]=0;
      inValue=1;
      t=0;
    } else if(stringBuf[s]=='\n') {
      value[t]=0;
      inValue=0;
      t=0;
      result=processLine(keyword,value);
      if(result<0) return result;
      lines++;
    } else {
      if(inValue) {
        value[t]=stringBuf[s];
      } else {
        keyword[t]=stringBuf[s];
      }
      t++;
    }
  }
  return 0;
}

