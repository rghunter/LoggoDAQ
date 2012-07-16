#ifndef conparse_h
#define conparse_h

//references to configuration parameters
extern int uartMode[2];
extern int baud[2];
extern char trigger[2];
extern char frameEnd[2];
extern int rawSize[2];
extern char timestamp[2];
extern int adcMode;
extern int adcFreq;
extern int adcBin;
extern char channelActive[9];
extern int nChannels;
extern int hasStartTime;
extern int startY, startM, startD, startH, startN, startS;
extern int GPSSyncMode;
extern int powerSave;

int readLogCon(void);

#define UART_NONE 0
#define UART_NMEA 1
#define UART_TEXT 2
#define UART_RAW 3
#define UART_SIRF 4
#define UART_BOTH 5

#define ADC_NONE 0
#define ADC_NMEA 1
#define ADC_BINARY 2
#define ADC_SIRF 3
#define ADC_TEXT 4

#define GPS_SYNC_LOW 1
#define GPS_SYNC_PPS 2

#endif
