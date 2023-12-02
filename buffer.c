// buffer_module.c
#include "buffer.h"

#define BUFFER_SIZE 2000

int buffer[BUFFER_SIZE];
int bufferIndex = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;