//A2DReadings.h
//Module to get different reading and values
#ifndef A2D_READINGS_H
#define A2D_READINGS_H

#include <stdlib.h>
#include <stdio.h>

#define A2D_FILE_VOLTAGE "/sys/bus/iio/devices/iio:device0/in_voltage"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int getVoltageReading(int node);

#endif