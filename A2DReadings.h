//A2DReadings.h
//Module to get different reading and values
#ifndef A2D_READINGS_H
#define A2D_READINGS_H
#include <stdio.h>

#define A2D_FILE_VOLTAGE "/sys/bus/iio/devices/iio:device0/in_voltage"

int getVoltageReading(int node);

#endif