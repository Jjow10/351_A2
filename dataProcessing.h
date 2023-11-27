//dataProcessing.h

#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <pthread.h>
#include <math.h>
#include "timeFunctions.h"
#include "A2DReadings.h"
#include "buttonState.h"
#include "buffer.h"

// Function to read the photoresistor and store the value in the buffer
void* readPhotoresistor(void* arg);
static int extractAndProcessSamples(void); 

#endif