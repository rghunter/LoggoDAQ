#ifndef command_h
#define command_h

#include "setup.h"
#ifdef UART_SYS

void writeCommand(void);
int loadCommands(void);

#endif

#endif
