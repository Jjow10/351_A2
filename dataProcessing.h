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
#include "photoresistorState.h"

#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

//Function to extract and process samples by the analysis module
int extractAndProcessSamples(void); 

#endif
