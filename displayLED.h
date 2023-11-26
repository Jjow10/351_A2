//displayLED.h
//Module to display values on 8x8 LED matrix
#ifndef DISPLAYLED_H
#define DISPLAYLED_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"
#define LED_DISPLAY_ADDRESS 0X70

//Initialize i2c bus
int initI2cBus(char* bus, int address);
//Write to slave register
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
//Initialize 8x8 LED Matrix registers
void initDisplay();
//Display inputted integer on 8x8 LED Matrix
void displayIntVal(int numToDisplay);
//Display inputted Double on 8x8 LED Matrix
void displayDoubleVal(double numToDisplay);


#endif
