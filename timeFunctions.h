//timeFunctions.h
//Module for all time related functions
#ifndef TIME_FUNCTIONS_H
#define TIME_FUNCTIONS_H_H
#include <stdio.h>
#include <errno.h>
#include <time.h>

void sleepForMs(long long delayInMs);

void sleepForUs(long long delayInUs);

long long getTimeinUs(void);

#endif