//photoresistorState.h
//Module for reading the photoresistor value and store in buffer
#ifndef PHOTORESISTOR__STATE_H
#define PHOTORESISTOR__STATE_H
#include <pthread.h>
#include "timeFunctions.h"
#include "A2DReadings.h"

#define BUFFER_SIZE 2000 // Adjust the size based on your needs

int buffer[BUFFER_SIZE];
int bufferIndex  = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

// Function to read the photoresistor and store the value in the buffer
void* readPhotoresistor(void* arg);

#endif
