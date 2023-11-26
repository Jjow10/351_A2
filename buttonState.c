#include "buttonState.h"

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