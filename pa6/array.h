#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE 8
#define MAX_NAME_LENGTH 24

#include <pthread.h>

// Bounded buffer problem
/*
Essentially, this interface defines a shared array that stores hostnames as C strings. 
The size of your array and the maximum size string that your array can handle should be 
defined with C macros in your array.h 
*/
typedef struct {
    char buffer[ARRAY_SIZE][MAX_NAME_LENGTH];           // the buffer
    int count;                                          // number of elements in the buffer
    pthread_mutex_t M;
    pthread_cond_t DC, DP;
} array;


int  array_init(array *s);                              // initialize the array
void* array_put (array *s, char *hostname);              // place element into the array, block when full
void*  array_get (array *s, char **hostname);             // remove element from the array, block when empty
void array_free(array *s);                                      // free the array's resources

#endif