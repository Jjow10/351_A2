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
    long long milliSeconds = (seconds * 1000000  + nanoSeconds / 1000);
    // float milliSeconds_3decimal = (float)milliSeconds;
    // printf( "second = %lld , nanosec = %lld \n", seconds , nanoSeconds);
    // printf( "ms = %lld   ms = %f \n", milliSeconds, milliSeconds_3decimal);
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
    int a2d_exp_average = 0;
    int a2d_previous_average  = 0;
    int total_run = -1;
    static int extractAndProcessSamples() {
    int readingCount = 1;
    //int Dips = 0;
    int a2d_min , a2d_max = 0;
    int a2d_average = 0;
    long long time_previous =0;
    long long time_current = 0;
    float time_diff =0 ;
    float time_min =0;
    float time_max =0;
    float time_average =0;
    float time_total= 0;
   
    // Access the buffer in a thread-safe manner
    pthread_mutex_lock(&bufferMutex);
    for (int i = 0; i < bufferIndex; i += 2) {
        int a2dReading = (buffer[i]);
        //long long timestamp = buffer[i + 1];        
        a2d_average = a2d_average + a2dReading;
       
        //getting time interval
        time_previous = time_current;
        time_current = getTimeInMs();
        //printf( "previous = %lld , current = %lld \n", time_previous,time_current);
        if(readingCount ==2){
            time_diff = (float)(time_current - time_previous);
        //    time_diff = time_diff / 1000;
            time_total = time_total + time_diff;
            printf ("timemin : %f   timemax : %f   timediff : %f   timetotal : %f \n",time_min, time_max , time_diff, time_total);
        }
        else if(readingCount > 2 ){
            time_diff = (float)(time_current - time_previous);
        //    time_diff = time_diff / 1000;
            if(time_min == 0){
                time_min = time_diff;
                time_max = time_diff;
            }
            else if(time_diff < time_min)
                time_min = time_diff;
            else if (time_diff > time_max)
                time_max = time_diff;
            time_total = time_total + time_diff;
            printf ("timemin : %f   timemax : %f   timediff : %f   timetotal : %f \n",time_min, time_max , time_diff, time_total);
        }

        if(a2d_min == 0){ // inital min,max = first voltage value
            a2d_min = a2dReading;
            a2d_max = a2dReading;
        }
        else{ // Decide if the new voltage is min or max
            if (a2dReading > a2d_max)
                a2d_max = a2dReading;
            else if ( a2dReading < a2d_min)
                a2d_min = a2dReading;            
        }
        readingCount++;
        //Process the sample (replace this with your actual processing logic)
        if(readingCount % 50 == 0 && isUserButtonPressed()) {
            return 1;
        }
    }
    total_run++;
    time_average = time_total /  (readingCount);
    a2d_average = (a2d_average / (readingCount)); 
    if (total_run == 1) //if this is the first run
        a2d_exp_average = a2d_average;
    else //expotential averaging, weighting the previous average at 99.9%
        a2d_exp_average = (0.999*a2d_previous_average) + (0.001 * a2d_average);
    
    if(total_run >=1)
    printf("Run #%d  Interval ms (%.3f, %.3f) avg = %.3f   Samples V(%d, %d) avg = %d   #Dips : %d   #Samples : %d \n", total_run,time_min,time_max,time_average,a2d_min,a2d_max,a2d_exp_average,4,readingCount);
 
    pthread_mutex_unlock(&bufferMutex);

    // Clear the buffer and restart filling it
    pthread_mutex_lock(&bufferMutex);
    bufferIndex = 0;
    pthread_mutex_unlock(&bufferMutex);
    a2d_previous_average = a2d_average;
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