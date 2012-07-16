#include <string.h>
#include "stringex.h"

char hexDigits[]="0123456789ABCDEF";

//an ascii character is blank if it is space or less, or greater than ~
int isBlank(char c) {
  if(c<=' ') return 1;
  if(c>'~') return 1;
  return 0;
}

//trim blank characters off the left end of a string in-place
void ltrim(char* s) {
  int i=0,len=0;
  while(s[len]>0 && isBlank(s[len])) len++;
  while(s[len+i]!=0) {
    s[i]=s[len+i];i++;
  }
  s[i]=0;
}
//trim blank characters off the right end of a string in-place
void rtrim(char* s) {
  int i=strlen(s)-1;
  while(isBlank(s[i]) && i>0) {
    s[i]=0;
    i--;
  }
}
//return the upper-case version of a character. Result is only
//different if c is lower-case
char upper(char c) {
  if(c>='a' && c<='z') return c-32;
  return c;
}
//trim blank characters off both ends of a string in-place
void trim(char* s) {
  ltrim(s);
  rtrim(s);
}
//get the integer value of a decimal number string
int dtoi(char* d) {
  int result=0;
  int len=strlen(d);
  for(int i=0;i<len;i++) {
    result=result*10+(d[i]-'0');
  }
  return result;
}
//get the integer value of a hex number string, signified by starting with 0x, case-insensitive
int htoi(char* d) {
  int result=0;
  int len=strlen(d);
  for(int i=2;i<len;i++) { //Skip the 0x at the beginning
    int digit=upper(d[i])-'0';
    if(digit>10) digit-=7;
    result=(result<<4)+digit;
  }
  return result;
}
//get integer value of either a hex string or a decimal string
int stoi(char *s) {
  int result;
  //if the number is a 1 digit decimal, s[1] will be \0, so ok
  if(upper(s[1])=='X') {
    result=htoi(s);
  } else {
    result=dtoi(s);
  }
  return result;
}

void toDec(char* buf, int in) {
  int p=0;
  if(in<0) {
    buf[p]='-';p++;
    in=-in;
  }
  if(in==0) {
    buf[p]='0';p++;
    buf[p]=0;
    return;
  }
  char dbuf[10];
  int i=0;
  while(in>0) {
    dbuf[i]=(in%10)+'0';
    in/=10;
    i++;
  }
  for(int j=i-1;j>=0;j--) {
    buf[p]=dbuf[j];p++;
  }
  buf[p]=0;
}


void to0Dec(char* buf, int in, int len) {
  int p=0;
  if(in<0) {
    buf[p]='-';p++;
    in=-in;
  }
  if(in==0) {
    for(p=0;p<len;p++)buf[p]='0';
    buf[p]=0;
    return;
  }
  char dbuf[10];
  int i=0;
  while(in>0) {
    dbuf[i]=(in%10)+'0';
    in/=10;
    i++;
  }
  while(i<len && i<10) {
    dbuf[i]='0';
    i++;
  }
  for(int j=i-1;j>=0;j--) {
    buf[p]=dbuf[j];p++;
  }
  buf[p]=0;
}


