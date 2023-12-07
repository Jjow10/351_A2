#include "timeFunctions.h"

void sleepForMs(long long delayInMs){ //Timesleep for 1ms for the delays.
   
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
   
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    
    struct timespec reqDelay = {seconds, nanoseconds};
    while (nanosleep(&reqDelay, &reqDelay) == -1) {
        if (errno == EINTR) { // Sleep was interrupted; continue sleeping with the remaining time.
            continue;
        } 
        else{
            printf("ERROR: nanosleep was interfered.\n");
            break; // Exit the loop on other errors.
        }
    }
}

void sleepForUs(long long delayInUs){ //Timesleep for 1ms for the delays.
   
    const long long US_PER_MS = 1000;
    const long long US_PER_SECOND = 1000000;
   
    long long delayUs = delayInUs * US_PER_MS;
    int seconds = delayUs / US_PER_SECOND;
    int microseconds = delayUs % US_PER_SECOND;
    
    struct timespec reqDelay = {seconds, microseconds};
    while (nanosleep(&reqDelay, &reqDelay) == -1) {
        if (errno == EINTR) { // Sleep was interrupted; continue sleeping with the remaining time.
            continue;
        } 
        else{
            printf("ERROR: microsleep was interfered.\n");
            break; // Exit the loop on other errors.
        }
    }
}

long long getTimeinUs(void){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long microSeconds = (seconds * 1000000  + nanoSeconds * 0.001);
    // double microSeconds_3decimal = (double)microSeconds;
    // printf( "second = %lld , nanosec = %lld \n", seconds*1000000 , nanoSeconds /1000);
    // printf( "ms = %lld   ms = %f \n", microSeconds, microSeconds_3decimal);
    return microSeconds;
}