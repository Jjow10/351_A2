//photoresistorState.h
//Module for reading the photoresistor value and store in buffer
#ifndef PHOTORESISTOR__STATE_H
#define PHOTORESISTOR__STATE_H
#include <pthread.h>
#include "timeFunctions.h"
#include "A2DReadings.h"

// Function to read the photoresistor and store the value in the buffer
void* readPhotoresistor(void* arg);

#endif
