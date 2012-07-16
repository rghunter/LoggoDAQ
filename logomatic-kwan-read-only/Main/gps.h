#ifndef gps_h
#define gps_h

#include "circular.h"

//GPS status
extern int GPSLight;
extern int GPShasPos;
//Latitude and longitude in 100ndeg (1e7 ndeg in 1 deg)
//Alt in cm
extern int lat;
extern int lon;
extern int alt;  //only works for SiRF for now

void parseNmea(circular* sirfBuf);
void parseSirf(circular* sirfBuf);

#endif
