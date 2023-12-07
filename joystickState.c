#include "joystickState.h"

#define BUFFER_SIZE 2000
#define JOYSTICK_BUTTON_X_VOLTAGE_RAW "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define JOYSTICK_BUTTON_Y_VOLTAGE_RAW "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"

/**
 * Gets the current position of the joystick.
 * Returns an integer between 0 and 4 inclusive.
 * 0 = center
 * 1 = up
 * 2 = down
 * 3 = left
 * 4 = right
*/
int getJoystickDirection() {
    char buffX[BUFFER_SIZE];
    char buffY[BUFFER_SIZE];

    FILE *joystickXVoltageFile = fopen(JOYSTICK_BUTTON_X_VOLTAGE_RAW, "r");
    if (joystickXVoltageFile == NULL) {
        printf("ERROR OPENING %s. Joystick may not be connected.", JOYSTICK_BUTTON_X_VOLTAGE_RAW);
        exit(1);
    }


    FILE *joystickYVoltageFile = fopen(JOYSTICK_BUTTON_Y_VOLTAGE_RAW, "r");
    if (joystickXVoltageFile == NULL) {
        printf("ERROR OPENING %s. Joystick may not be connected.", JOYSTICK_BUTTON_Y_VOLTAGE_RAW);
        exit(1);
    }

    // Read X value into buffX
    fflush(joystickXVoltageFile);
    fseek(joystickXVoltageFile, 0, SEEK_SET);
    fgets(buffX, 1024, joystickXVoltageFile);
    rewind(joystickXVoltageFile);
    fclose(joystickXVoltageFile);

    // Read Y value into buffY
    fflush(joystickYVoltageFile);
    fseek(joystickYVoltageFile, 0, SEEK_SET);
    fgets(buffY, 1024, joystickYVoltageFile);
    rewind(joystickYVoltageFile);
    fclose(joystickYVoltageFile);

    int rawXVoltage = atoi(buffX);
    int rawYVoltage = atoi(buffY);

    // Center
    if (rawXVoltage > 1900 && rawXVoltage < 2000 && rawYVoltage < 2150 && rawYVoltage > 1950) {
        return 0;
    }
    // Up
    else if (rawYVoltage >= 2500) {
        return 1;
    }
    // Down
    else if (rawYVoltage <= 1500) {
        return 2;
    }
    // Left
    else if (rawXVoltage >= 2500) {
        return 3;
    }
    // Right
    else if (rawXVoltage <= 1800) {
        return 4;
    }
    // ERROR, how did you get here?
    else {
        return 500;
    }
}