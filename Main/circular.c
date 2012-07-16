#include <stdlib.h>
#include "circular.h"
#include "stringex.h"
#include "conparse.h"
#include "nmeawrite.h"

int isFull(circular *buf) {
  return (buf->head+1)%1024==buf->tail;
}

int isEmpty(circular *buf) {
  return buf->head==buf->tail;
}

//returns 0 if character written, negative
//if character could not be written
int fill(circular *buf,char in) {
  if(!isFull(buf)) {
    buf->buf[buf->head]=in;
    buf->head=(buf->head+1)%1024;
    return 0;
  }
  return -1;
}

char get(circular *buf) {
  if(isEmpty(buf)) return 0;
  char result=buf->buf[buf->tail];
  buf->tail=(buf->tail+1)%1024;
  return result;
}

int fillString(circular* buf, char* in) {
  int i=0;
  while(in[i]!=0) {
    int result=fill(buf,in[i]);
    if (result<0) return result;
    i++;
  }
  return 0;
}

int fillDec(circular* buf, int in) {
  char dbuf[10];
  toDec(dbuf,in);
  fillString(buf,dbuf);
  return 0;
}

int fill0Dec(circular* buf, int in, int len) {
  char dbuf[10];
  to0Dec(dbuf,in,len);
  fillString(buf,dbuf);
  return 0;
}

int fillHex(circular* buf, unsigned int in, int len) {
  int hexit;
  int result;
  for(int i=0;i<len;i++) {
    hexit=(in>>(4*(len-i-1))) & 0x0F;
    result=fill(buf,hexDigits[hexit]);
    if(result!=0) return result;
  }
  return 0;
}

int fillStringn(circular* buf, char* in, int len) {
  for(int i=0;i<len;i++) {
    int result=fill(buf,in[i]);
    if(result<0) return result;
  }
  return 0;
}

int fillShort(circular* buf, short in) {
  int result=fill(buf,(in>> 8) & 0xFF);
  if(result<0) return result;
  return     fill(buf,(in>> 0) & 0xFF);
}

int fillInt(circular* buf, int in) {
  int result=fillShort(buf,(in>> 16) & 0xFFFF);
  if(result<0) return result;
  return     fillShort(buf,(in>>  0) & 0xFFFF);
}

char peekMid(circular* buf, int pos) {
  pos+=buf->mid;
  pos&=0x3FF;
  return buf->buf[pos];
}

short peekMidShort(circular* buf, int pos) {
  return (((short)peekMid(buf,pos))<<8) | peekMid(buf,pos+1);
}

int peekMidInt(circular* buf, int pos) {
  return (((int)peekMidShort(buf,pos))<<16) | peekMidShort(buf,pos+2);
}
  
char peekHead(circular* buf, int pos) {
  pos+=buf->head;
  pos&=0x3FF;
  return buf->buf[pos];
}

char peekTail(circular* buf, int pos) {
  pos+=buf->tail;
  pos&=0x3FF;
  return buf->buf[pos];
}

void pokeMid(circular* buf, int pos, char poke) {
  pos+=buf->mid;
  pos&=0x3FF;
  buf->buf[pos]=poke;
}

void pokeHead(circular* buf, int pos, char poke) {
  pos+=buf->head;
  pos&=0x3FF;
  buf->buf[pos]=poke;
}

void pokeTail(circular* buf, int pos, char poke) {
  pos+=buf->tail;
  pos&=0x3FF;
  buf->buf[pos]=poke;
}

void mark(circular* buf) {
  buf->mid=buf->head;
}

int unreadylen(circular* buf) {
  int h=buf->head;
  int m=buf->mid;
  if(h<m) h+=1024;
  return h-m;
}

int readylen(circular* buf) {
  int m=buf->mid;
  int t=buf->tail;
  if(m<t) m+=1024;
  return m-t;
}

void fillError(circular* to, char* msg, unsigned int a, unsigned int b) {
  switch(adcMode) {
    case ADC_NMEA:
	  fillStartNMEA(to,'E');
	  fillString(to,msg);
	  fill(to,' ');
	  fillHex(to,a,8);
	  fill(to,' ');
	  fillHex(to,b,8);
	  fillFinishNMEA(to);
	  break;
  }
}

int drain(circular* from, circular* to) {
  while(readylen(from)>0) {
    if(isFull(to)) {
	  empty(to);
	  empty(from);
	  fillError(to,"Buffer full",from,to);
	  return -1;
	}
    fill(to,get(from));
  }
  mark(to);
  return 0;
}

void empty(circular* buf) {
  buf->head=0;
  buf->tail=0;
  buf->mid=0;
}
