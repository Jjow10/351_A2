#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#define A2D_FILE_VOLTAGE "/sys/bus/iio/devices/iio:device0/in_voltage"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define BUFFER_SIZE 2000 // Adjust the size based on your needs

static void sleepForMs(long long delayInMs){ //Timesleep for 1ms for the delays.
   
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

static long long getTimeInMs(void){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = 1121999606 + seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}


int getVoltageReading(int node){
    // Open file
    char A2D_to_open[50];
    sprintf(A2D_to_open,"%s%i_raw",A2D_FILE_VOLTAGE,node);

    FILE *f = fopen(A2D_to_open, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }

    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);

    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }

    // Close file
    fclose(f);
    return a2dReading;
}

int buffer[BUFFER_SIZE];
int bufferIndex = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

// Function to read the photoresistor and store the value in the buffer
void* readPhotoresistor(void* arg) {
    while (1) {
        // Simulate reading A2D (replace this with your actual A2D reading)
        int a2dReading = getVoltageReading(1);

        // Get current timestamp
        long long timestamp = getTimeInMs();

        // Store the reading and timestamp in the buffer
        pthread_mutex_lock(&bufferMutex);
        buffer[bufferIndex++] = a2dReading;
        buffer[bufferIndex++] = timestamp;
        pthread_mutex_unlock(&bufferMutex);

        // Sleep for 1ms
        sleepForMs(1);
    }

    return NULL;
}


// Function to extract and process samples by the analysis module
void extractAndProcessSamples() {

    int Readingcount = 0;
    // Access the buffer in a thread-safe manner
    pthread_mutex_lock(&bufferMutex);
    for (int i = 0; i < bufferIndex; i += 2) {
        int a2dReading = buffer[i];
        long long timestamp = buffer[i + 1];
        Readingcount++;
        // Process the sample (replace this with your actual processing logic)
        printf("#%d: A2D Reading = %d, Timestamp = %lld\n", Readingcount, a2dReading, timestamp);
    }
    pthread_mutex_unlock(&bufferMutex);

    // Clear the buffer and restart filling it
    pthread_mutex_lock(&bufferMutex);
    bufferIndex = 0;
    pthread_mutex_unlock(&bufferMutex);
}


int main(){
    pthread_t photoresistorThread;
    // Create a thread to read the photoresistor
    if (pthread_create(&photoresistorThread, NULL, readPhotoresistor, NULL) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
 




    while(true){
        extractAndProcessSamples();
        sleepForMs(1000);
    
        // int reading = getVoltageReading(2);
        // double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        // printf("Value %5d ==> %5.2fV\n", reading, voltage);
    }
return 0;
}

