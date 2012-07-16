//i2c.h - header file for Logomatic bit-banging I2C protocol
#ifndef i2c_h
#define i2c_h

//A bit-banging implementation of the master part of the I2C two-wire protocol using any
//two general purpose I/O pins in P0.XX. Intended for the Sparkfun Logomatic since they 
//wired the actual I2C port to the lights and buttons :(

//I2C timing: The bus frequency is more of what you'd call "guidelines" than actual rules.
//It represents a maximum rate, and in this implementation, is implemented by a busy wait.
//The wait is not compensated for overhead while actually doing the I2C bitbang, so it never
//really goes as fast as you ask. Also since this is the master, if it gets delayed by an
//interrupt, no biggie, since the slave has to follow the clock. This implementation
//respects clock stretches by slaves

//Use pin P0.17 (Pin 47, SCK) for SCL and pin P0.18 (Pin 53, MISO) for SDA on a logomatic

//Setup a bit-bang I2C port using two digital I/O pins
//input
//  PCL - Pin P0.XX number to use as SCL clock line
//  PDA - Pin P0.XX number to use as SDA data line
//  freq - Approximate bus frequency in Hz
//return
//  0 is success
//  some other number for failure
int i2c_setup(int PCL, int PDA, int freq);

//Release the bit-bang I2C port, setting the used pins to hi-Z
//return
//  0 is success
//  some other number for failure
int i2c_stop(void);

//Transmit a string on the I2C port
//input
//  addr - slave address to send to 
//  buf - pointer to byte string to send
//  len - number of bytes to send
//return
//  0 is success
//  some other number for failure
int i2c_tx_string(unsigned char addr, char* buf, int len);

//Receive a string on the I2C port
//input
//  addr - slave address to receive from
//  buf - pointer to byte buffer to receive
//  len - number of bytes to receive
//return
//  0 is success
//  some other number for failure
int i2c_rx_string(unsigned char addr, char* buf, int len);

#endif
