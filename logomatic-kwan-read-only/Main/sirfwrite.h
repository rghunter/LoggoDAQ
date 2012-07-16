#ifndef sirfwrite_h
#define sirfwrite_h
#include "circular.h"

void fillStartSirfRaw(circular* buf);
void fillStartSirf(circular* buf, char PktId);
void fillFinishSirf(circular* buf);
void fillFinishSirfSend(circular* buf, int port);

#endif

