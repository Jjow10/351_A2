//buttonState.h
//Module to read input of button
#ifndef BUTTON_STATE_H
#define BUTTON_STATE_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define USER_BUTTON_VALUE_FILE_PATH "/sys/class/gpio/gpio72/value"

static bool isUserButtonPressed();

#endif