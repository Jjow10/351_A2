#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define A2D_FILE_VOLTAGE "/sys/bus/iio/devices/iio:device0/in_voltage"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

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


int main(){
    while(true){
        int reading = getVoltageReading(2);
        double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        printf("Value %5d ==> %5.2fV\n", reading, voltage);
    }
return 0;
}