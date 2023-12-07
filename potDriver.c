//potDriver.c
//Contains the main function to run program
#include <pthread.h>
#include <math.h>
#include "joystickState.h"
#include "displayLED.h"
#include "buttonState.h"
#include "timeFunctions.h"
#include "A2DReadings.h"

#define BUFFER_SIZE 2000 // Adjust the size based on your needs

long long buffer[BUFFER_SIZE];
int bufferIndex  = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

//run linux terminal commands
void runCommand(char* command){
    FILE *pipe = popen(command, "w");
    char buffer[1024];
    while(!feof(pipe) && !ferror(pipe)){
        if(fgets(buffer, sizeof(buffer), pipe) == NULL){
            break;
        }   
    }
    int exitCode = WEXITSTATUS(pclose(pipe));
    if(exitCode != 0){
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

// Function to read the photoresistor and store the value in the buffer
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

double a2d_exp_average  = 0, a2d_previous_average  = 0;
int total_run = 0; 
double a2d_dip_average;
int prevDipCount = 0;
double prevA2d_min = 0;
double prevA2d_max = 0;
double prevTime_interval_min = 0;
double prevTime_interval_max = 0;

static int extractAndProcessSamples() {
    // Function to extract and process samples by the analysis module
    int readingCount = 0; //tracking how many samples are collected each run
    double a2d_min = 0 , a2d_max  = 0, a2d_average  = 0; //A2D variabls
    double time_interval = 0 ,time_interval_min = 0, time_interval_max = 0, time_average = 0,time_total = 0;
    bool dipDetected = false; // Dip variables
    double dipThreshold = 0.1, hysteresis = 0.03;
    int dipCount = 0;

    int joystickDirection = getJoystickDirection();
    initDisplay();
    // Center (default behaviour)
    // Display integer number of dips on LED
    if (joystickDirection == 0) {
        displayIntVal(prevDipCount);
    }
    // Up
    // Display floating point maximum sample voltage (V)
    else if (joystickDirection == 1) {
        displayDoubleVal(prevA2d_max);
    }
    // Down
    // Display floating point minimum sample voltage (V)
    else if (joystickDirection == 2) {
        displayDoubleVal(prevA2d_min);
    }
    // Left
    // Display floating point minimum interval between samples (ms)
    else if (joystickDirection == 3) {
        displayDoubleVal(prevTime_interval_min);
    }
    // Right
    // Display floating point maximum interval between samples (ms)
    else if (joystickDirection == 4) {
        displayDoubleVal(prevTime_interval_max);
    }

    // Access the buffer in a thread-safe manner
    pthread_mutex_lock(&bufferMutex);
    for (int i  = 0; i < bufferIndex; i += 2) {
        double a2dReading = ((double)buffer[i] / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;    
        a2d_average += a2dReading;

        sleepForUs(50);
        if ((buffer[i+3] > buffer[i+1]) && (buffer[i+3] > 0)) {
            time_interval = (double)(buffer[i+3] - buffer[i+1]);
            time_interval *= 0.001;
            if (readingCount == 0) { //First round
                time_total += time_interval;
            } 
            else if (readingCount >= 1) { //From Second round, start recording the Time Interval min/max
                if (time_interval_min == 0 && time_interval_max == 0){
                    time_interval_min = time_interval;
                    time_interval_max = time_interval;
                }
                else if (time_interval < time_interval_min) {
                    time_interval_min = time_interval;
                }
                else if (time_interval > time_interval_max) {
                    time_interval_max = time_interval;
                }
                time_total += time_interval;
            }
        }

        // getting min/max voltage
        if (a2d_min == 0 || a2d_max == 0) { // inital min,max = first voltage value
            a2d_min = a2dReading;
            a2d_max = a2dReading;
        }
        else { // Decide if the new voltage is min or max
            if (a2dReading > a2d_max) {
                a2d_max = a2dReading;
            }
            else if (a2dReading < a2d_min) {
                a2d_min = a2dReading;
            }
        }
        
        // Dip detection logic
        if (readingCount == 0) {
            a2d_dip_average = a2dReading;
        }
        else {
            a2d_dip_average = 0.9*a2d_dip_average + 0.1*a2dReading;
        }
        double voltageDiff = fabs(a2d_dip_average - a2dReading);

        if (!dipDetected && voltageDiff >= dipThreshold) {
            // Dip detected
            dipDetected = true;
            dipCount++;
        }

        if (dipDetected && voltageDiff <= (dipThreshold - hysteresis)) {
            // Reset dip detection when voltage goes above the threshold + hysteresis
            dipDetected = false;
        }

        //Process the sample (check if the Userbutton is pressed every 50 reading counts)
        readingCount++;
        if(readingCount % 50 == 0 && isUserButtonPressed()) {
            return 1;
        }
    }
    
    //Calculations For Step 2.5
    time_average = time_total / (readingCount);
    a2d_average /= readingCount;
    
    if (total_run == 1) {
        a2d_exp_average = a2d_average;
    } //if this is the first run
    else {
        a2d_exp_average = (0.999*a2d_previous_average) + (0.001 * a2d_average);
    } //expotential averaging, weighting the previous average at 99.9%    
    
    //2.5 Printout Comment
    if (total_run >= 1)
    printf("Run #%d  Interval ms (%.3f, %.3f) avg = %.3f   Samples V(%.2f, %.2f) avg = %.2f   #Dips : %d   #Samples : %d \n", total_run,time_interval_min,time_interval_max,time_average,a2d_min,a2d_max,a2d_exp_average,dipCount,readingCount);

    // End of the 2.5 module
    pthread_mutex_unlock(&bufferMutex);

    // Clear the buffer and restart filling it
    pthread_mutex_lock(&bufferMutex);
    bufferIndex  = 0;
    pthread_mutex_unlock(&bufferMutex);
    prevDipCount = dipCount;
    prevA2d_max = a2d_max;
    prevA2d_min = a2d_min;
    prevTime_interval_min = time_interval_min;
    prevTime_interval_max = time_interval_max;
    a2d_previous_average = a2d_average;
    total_run++;
    return 0;
}

static void clean(pthread_t thread) {
    //1. Shutting down all running threads
    //2. Freeing all dynamically allocated memory
    //3. Exiting without any runtime errors (such as a segfault or divide by zero)

    pthread_cancel(thread);
}

int main(){
    runCommand("config-pin p8.43 gpio");
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");

    pthread_t photoresistorThread;
    // Create a thread to read the photoresistor
    if (pthread_create(&photoresistorThread, NULL, readPhotoresistor, NULL) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    // While the USER button has not been pressed, keep processing values.
    while(!extractAndProcessSamples()){
        sleepForMs(1000);
    }

    clean(photoresistorThread);

    return 0;
}