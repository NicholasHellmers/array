#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE 8
#define MAX_NAME_LENGTH 900

// #include <pthread.h>
#include <semaphore.h>

// Bounded buffer problem
/*
Essentially, this interface defines a shared array that stores hostnames as C strings. 
The size of your array and the maximum size string that your array can handle should be 
defined with C macros in your array.h 
*/
typedef struct {
    char *buf[ARRAY_SIZE];          // the buffer
    int count;                      // number of elements in the buffer
} array;

sem_t mutex;                    // mutual exclusion
sem_t empty;                    // empty
sem_t full;                     // full

int  array_init(array *s);                   // initialize the array
int  array_put (array *s, char *hostname);   // place element into the array, block when full
int  array_get (array *s, char **hostname);  // remove element from the array, block when empty
void array_free(array *s);                   // free the array's resources

#endif