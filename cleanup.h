//cleanup.h
//Header file for cleanup.c providing clean up functions
#ifndef CLEANUP_H
#define CLEANUP_H

//1. Shutting down all running threads
//2. Freeing all dynamically allocated memory
//3. Exiting without any runtime errors (such as a segfault or divide by zero)
void clean(void);

#endif