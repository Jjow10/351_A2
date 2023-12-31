#include "A2DReadings.h"

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
    int a2dReading  = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);

    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }

    // Close file
    fclose(f);
    return a2dReading;
}