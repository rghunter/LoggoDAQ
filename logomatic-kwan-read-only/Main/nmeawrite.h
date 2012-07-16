#ifndef nmeawrite_h
#define nmeawrite_h
#include "circular.h"

void fillStartNMEA(circular* buf, char header);
void fillStartNMEAraw(circular* buf);
void fillFinishNMEA(circular* buf);
void fillFinishNMEASend(circular* buf, int port);

#endif

