#ifndef adc_h
#define adc_h

#include "circular.h"
#include "setup.h"

#ifdef ADC_SYS

extern circular adcBuf;

void writeADCsetup(void);
void startRecordADC(void);

#endif

#endif
