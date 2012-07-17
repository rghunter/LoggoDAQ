/*
 * ADXL345.c
 *
 *  Created on: Jul 16, 2012
 *      Author: rghunter
 */

#include "ADXL345.h"
#include "twi.h"
#include "type.h"

static unsigned int _address;

unsigned char init;

//Private Functions:

int _ADXL345_checkID(void)
{
	if(i2c_read_reg(ACCEL_ADDR,ADXL345_DEVIDREG) == ADXL345_DEVID)
		return FALSE; //Correct Device ID!
	else
		return TRUE; //sensor did not return correct Device ID
}

//Public Functions


int ADXL345_init(void)
{
	i2c_setup(17,18,400000); //setup SCLk as SCL and and MISO as SDA with bus frequency of 400kHz
	if(_ADXL345_checkID())
		return TRUE;	//did not intialize properly
	init = TRUE;
	return FALSE;
}



void ADXL354_powerOn(void)
{

}
