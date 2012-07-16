#ifndef load_h
#define load_h

#define LOAD_UART  0
#define LOAD_ADC   1
#define LOAD_FLUSH 2
#define LOAD_LOAD  3

int countLoad(void);
void hasLoad(char task);
void clearLoad(void);
void resetLoad(void);

#endif
