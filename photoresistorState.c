#include "photoresistorState.h"

#define BUFFER_SIZE 2000 // Adjust the size based on your needs

int buffer[BUFFER_SIZE];
int bufferIndex  = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

void* readPhotoresistor(void* arg) {
    while (1) {
        // Read A2D
        int a2dReading = getVoltageReading(1);

        // Get current timestamp
        long long timestamp = getTimeinUs();

        // Store the reading and timestamp in the buffers
        pthread_mutex_lock(&bufferMutex);
        buffer[bufferIndex++] = a2dReading;
        buffer[bufferIndex++] = timestamp;
        pthread_mutex_unlock(&bufferMutex);

        // Sleep for 1ms
        sleepForMs(1);
    }

    return NULL;
}