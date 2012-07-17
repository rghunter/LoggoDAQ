/*
 * ADXL345.h
 *
 *  Created on: Jul 16, 2012
 *      Author: rghunter
 */

#ifndef ADXL345_H_
#define ADXL345_H_


//Device Address
#define ACCEL_ADDR 0x53

//Device ID
#define ADXL345_DEVID 0xE5

//Register Address
#define ADXL345_DEVIDREG 0x00

//Public Functions
int ADXL345_init(void);
void ADXL345_powerOn(void);


#endif /* ADXL345_H_ */
