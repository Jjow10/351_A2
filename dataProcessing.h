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

//Function to extract and process samples by the analysis module
int extractAndProcessSamples(void); 

#endif