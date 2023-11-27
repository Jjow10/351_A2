// buffer.h

#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>

#define BUFFER_SIZE 2000

extern int buffer[BUFFER_SIZE];
extern int bufferIndex;
extern pthread_mutex_t bufferMutex;

#endif // BUFFER_H
