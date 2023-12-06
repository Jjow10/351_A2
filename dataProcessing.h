//dataProcessing.h
// Header file for dataProcessing.c providing function to extract and process samples by the analysis module
#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <pthread.h>
#include <math.h>
#include "timeFunctions.h"
#include "A2DReadings.h"
#include "buttonState.h"
#include "displayLED.h"
#include "joystickState.h"

#define BUFFER_SIZE 2000 // Adjust the size based on your needs

int buffer[BUFFER_SIZE];
int bufferIndex  = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

// Function to read the photoresistor and store the value in the buffer
void* readPhotoresistor(void* arg);

//Function to extract and process samples by the analysis module
int extractAndProcessSamples(void); 

#endif
