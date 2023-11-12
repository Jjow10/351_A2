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
#define USER_BUTTON_VALUE_FILE_PATH "/sys/class/gpio/gpio72/value"
#define BUFFER_SIZE 2000 // Adjust the size based on your needs

static bool isUserButtonPressed();

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
static int extractAndProcessSamples() {
    int readingCount = 0;
    // Access the buffer in a thread-safe manner
    pthread_mutex_lock(&bufferMutex);
    for (int i = 0; i < bufferIndex; i += 2) {
        int a2dReading = buffer[i];
        long long timestamp = buffer[i + 1];
        readingCount++;
        // Process the sample (replace this with your actual processing logic)
        printf("#%d: A2D Reading = %d, Timestamp = %lld\n", readingCount, a2dReading, timestamp);

        if(readingCount % 50 == 0 && isUserButtonPressed()) {
            return 1;
        }
    }
    pthread_mutex_unlock(&bufferMutex);

    // Clear the buffer and restart filling it
    pthread_mutex_lock(&bufferMutex);
    bufferIndex = 0;
    pthread_mutex_unlock(&bufferMutex);

    return 0;
}

static bool isUserButtonPressed() {
    char buff[BUFFER_SIZE];

    FILE *pbuttonValueFile = fopen(USER_BUTTON_VALUE_FILE_PATH, "r");
    if (pbuttonValueFile == NULL) {
        printf("ERROR OPENING %s.", USER_BUTTON_VALUE_FILE_PATH);
        exit(1);
    }

    fflush(pbuttonValueFile);
    fseek(pbuttonValueFile, 0, SEEK_SET);
    fgets(buff, 1024, pbuttonValueFile);
    rewind(pbuttonValueFile);
    fclose(pbuttonValueFile);

    return atoi(buff) == 0 ? true : false;
}

static void clean() {
    //1. Shutting down all running threads
    //2. Freeing all dynamically allocated memory
    //3. Exiting without any runtime errors (such as a segfault or divide by zero)
}

int main(){
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

