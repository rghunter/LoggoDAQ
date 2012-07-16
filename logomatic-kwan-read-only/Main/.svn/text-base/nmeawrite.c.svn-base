#include "nmeawrite.h"
#include "circular.h"
#include "setup.h"
#include "uart.h"

void fillStartNMEA(circular* buf,char header) {
  fillString(buf,"$PKWN");
  fill(buf,header);
  fill(buf,',');
}

void fillStartNMEAraw(circular* buf) {
  fill(buf,'$');
}

static void fillFinishNMEACore(circular* buf) {
  int len=unreadylen(buf)-1;
  short checksum=0;
  for(int i=0;i<len;i++) {
    checksum^=peekMid(buf,i+1);
  }
  fill(buf,'*');
  fillHex(buf,checksum,2);
  fillShort(buf,0x0D0A);
}

void fillFinishNMEA(circular* buf) {
  fillFinishNMEACore(buf);
  mark(buf);
}
#ifdef COMMAND_SYS
void fillFinishNMEASend(circular* buf, int port) {
  fillFinishNMEACore(buf);
  sendBuf(port,buf);
}
#endif
