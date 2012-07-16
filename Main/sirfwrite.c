#include "sirfwrite.h"
#include "circular.h"
#include "uart.h"

void fillStartSirfRaw(circular* buf) {
  fillShort(buf,0xA0A2);
  fillShort(buf,0);
}

void fillStartSirf(circular* buf, char PktId) {
  fillStartSirfRaw(buf);
  fill(buf,PktId);
}

static void fillFinishSirfCore(circular* buf) {
  int len=unreadylen(buf)-4;
  pokeMid(buf,2,(len >> 8) & 0xFF);
  pokeMid(buf,3,(len >> 0) & 0xFF);
  short checksum=0;
  for(int i=0;i<len;i++) {
    checksum=(checksum+peekMid(buf,i+4)) & 0x7FFF;
  }
  fillShort(buf,checksum);
  fillShort(buf,0xB0B3);
}

void fillFinishSirf(circular* buf) {
  fillFinishSirfCore(buf);
  mark(buf);
}
#ifdef COMMAND_SYS
void fillFinishSirfSend(circular* buf, int port) {
  fillFinishSirfCore(buf);
  sendBuf(port,buf);
}
#endif
