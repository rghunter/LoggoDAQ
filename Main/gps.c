#include <stdlib.h>
#include <string.h>
#include "gps.h"
#include "conparse.h"
#include "circular.h"
#include "LPC214x.h"
#include "main.h"
#include "stringex.h"

//SiRF packet processing and GPS status
int GPSLight;
int GPShasPos;
//Latitude and longitude in 100ndeg (1e7 ndeg in 1 deg)
//Alt in cm
int lat,lon,alt;
int satMask,satNum;

void parseSirf(circular* sirfBuf) {
  if(peekMid(sirfBuf,4)==0x29) {
    //Check if we have a position
    GPShasPos=(peekMid(sirfBuf,6)==0) & (peekMid(sirfBuf,5)==0);
    if(GPShasPos) {
      GPSLight=1;
      if(GPSSyncMode==1) {
        YEAR=(unsigned)peekMidShort(sirfBuf,15);
        MONTH=peekMid(sirfBuf,17);
        DOM=peekMid(sirfBuf,18);
        HOUR=peekMid(sirfBuf,19);
        MIN=peekMid(sirfBuf,20);
        SEC=((unsigned)peekMidShort(sirfBuf,21))/1000;
        satMask=peekMidInt(sirfBuf,23);
        satNum=peekMid(sirfBuf,92);
        lat=peekMidInt(sirfBuf,27);
        lon=peekMidInt(sirfBuf,31);
      }
    }
  } else if(!GPShasPos && peekMid(sirfBuf,4)==4) {
    //Check if we have at least one sat
    if(peekMid(sirfBuf,12)>0) {
      GPSLight=1-GPSLight;
    } else {
      GPSLight=0;
    }
  }
}

static int parseCommaPart(circular* buf, int* p0, char* out) {
  int p=*p0;
  char b=peekMid(buf,p);
  while(p-*p0<15 && b!=',') {
    out[p-*p0]=b;
    p++;
    b=peekMid(buf,p);
  }
  //terminate the string
  out[p-*p0]=0;  
  //skip the comma
  p++;
  int result=p-*p0;
  *p0=p;
  return result;
}

static int skipCommaPart(circular* buf, int* p0) {
  int p=*p0;
  char b=peekMid(buf,p);
  while(p-*p0<15 && b!=',') {
    p++;
    b=peekMid(buf,p);
  }
  //skip the comma
  p++;
  int result=p-*p0;
  *p0=p;
  return result;
}

static char parseChar(circular* buf, int* p0) {
  //Hemisphere
  char result=peekMid(buf,*p0);
  if(result!=',') {
    (*p0)++;
  } else {
    result=0;
  }
  //skip the comma
  (*p0)++;
  return result;
}

//Given a string representing number with a decimal point, return the number multiplied by 10^(shift)
static int parseNumber(char* in, int* shift) {
  char buf[16];
  int len=strlen(in);
  int decimal=0;
  while(in[decimal]!='.' && in[decimal]!=0) decimal++;
  in[decimal]=0;
  int intlen=decimal;
  int fraclen=len-decimal-1;
  *shift=fraclen;
  for(int i=0;i<decimal;i++) buf[i]=in[i];
  for(int i=decimal+1;i<len;i++) buf[i-1]=in[i];
  buf[len-1]=0;
  return stoi(buf);
}

//Given a string representing a number in the form dddmm.mmmm, return
//an integer representing that number in just degrees multiplied by 10^7
static int parseMin(char* buf) {
  int shift;
  int num=parseNumber(buf,&shift);
  int shift10=1;
  for(int i=0;i<shift;i++) shift10*=10;
  int rshift=7-shift;
  int rshift10=1;
  for(int i=0;i<rshift;i++) rshift10*=10;
  int intpart=num/shift10;
  int fracpart=num%shift10;
  int degpart=intpart/100;
  int minintpart=intpart%100;
  int minpart=(minintpart*shift10+fracpart)*rshift10/60;
  return degpart*10000000+minpart;
}

static void parseGPRMC(circular* buf, int p) {
  char time[15]; //Good for at least microsecond precision
  char status;
  char latb[15]; //Good to absurd precision
  char latHem;
  char lonb[15]; //Good to absurd precision
  char lonHem;
  char date[7]; //It promises only 6 digits
  
  parseCommaPart(buf,&p,time);
  status=parseChar(buf,&p);
  parseCommaPart(buf,&p,latb);  
  latHem=parseChar(buf,&p);
  parseCommaPart(buf,&p,lonb);
  lonHem=parseChar(buf,&p);
  skipCommaPart(buf,&p); //speed  
  skipCommaPart(buf,&p); //course
  parseCommaPart(buf,&p,date);

  if(status=='A') {
    if(strlen(latb)>0) {
      GPShasPos=1;
      GPSLight=1;
    
      lat=parseMin(latb);
      if(latHem=='S') lat*=-1;
      lon=parseMin(lonb);
      if(lonHem=='W') lon*=-1;
    } else {
      GPSLight=1-GPSLight;
    }
  }

  if(GPSSyncMode==1) {
    //Record the time in the calendar registers  
    time[6]=0;
    int hns=stoi(time);
    int dmy=stoi(date);
    HOUR=hns/10000;
    MIN=(hns%10000)/100;
    SEC=hns%100;
    
    DOM=dmy/10000;
    MONTH=(dmy%10000)/100;
    YEAR=dmy%100+2000;
  }
}

void parseNmea(circular* buf) {
  int p=0;
  char tag[10];
  char b=peekMid(buf,p);
  while(p<10 && b!=',') {
    tag[p]=b;
    p++;
    b=peekMid(buf,p);
  }
  tag[p]=0;
  if(strcmp(tag,"$GPRMC")==0) {
    parseGPRMC(buf,p+1);
  } 
}

