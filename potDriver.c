#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "cleanup.h"
#include "dataProcessing.h"
#include "photoresistorState.h"


int main(void){
    pthread_t photoresistorThread;
    // Create a thread to read the photoresistor
    if (pthread_create(&photoresistorThread, NULL, readPhotoresistor, NULL) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    // While the USER button has not been pressed, keep processing values.
    while(!extractAndProcessSamples()){
        sleepForMs(1000);
        // int reading = getVoltageReading(2);
        // double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        // printf("Value %5d ==> %5.2fV\n", reading, voltage);
    }

    clean();

    return 0;
}