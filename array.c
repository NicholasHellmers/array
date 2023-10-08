#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "array.h"

/*

In this assignment you will build a thread-safe Shared Array, that will then be included in PA6 
to create a multi-threaded DNS resolver program.  To help you organize your code, we will require 
you to build a separate C module that deals solely with the shared array.  This modularization will 
allow you to build and test a shared array abstraction without having to worry about all the 
details of your PA6 solution. 

This approach is an application of the principal of Separation of Concerns.  In order to illustrate 
how to build a program as separate C modules, we'll need to review the relationship between an 
interface and an implementation.

*/

/*

typedef struct {
    char *buf[ARRAY_SIZE];  // the buffer
    sem_t mutex;                // mutual exclusion
    sem_t empty;                // empty
    sem_t full;                 // full
    int count;                  // the count of the number of items in the buffer
} array;

*/

int array_init(array *s) {                  // initialize the array
    pthread_mutex_init(&s->mutex, NULL);
    pthread_cond_init(&s->full, NULL);
    pthread_cond_init(&s->empty, NULL);
}
int array_put (array *s, char *hostname) {  // place element into the array, block when full
    do {
        pthread_mutex_lock(&s->mutex);
        while (s->count == ARRAY_SIZE) {
            pthread_cond_wait(&s->full, &s->mutex);
        }
        s->buf[s->count] = hostname;
        s->count++;
        pthread_cond_signal(&s->empty);
        pthread_mutex_unlock(&s->mutex);
    } while (0);
}
int array_get (array *s, char **hostname) { // remove element from the array, block when empty
    do {
        pthread_mutex_lock(&s->mutex);
        while (s->count == 0) {
            pthread_cond_wait(&s->empty, &s->mutex);
        }
        s->count--;
        *hostname = s->buf[s->count];
        pthread_cond_signal(&s->full);
        pthread_mutex_unlock(&s->mutex);
    } while (0);
}
void array_free(array *s) {                 // free the array's resources
    pthread_mutex_destroy(&s->mutex);
    pthread_cond_destroy(&s->full);
    pthread_cond_destroy(&s->empty);
}

int main() {
    array *s = malloc(sizeof(array));
    array_init(s);
    array_put(s, "www.google.com");
    array_put(s, "www.yahoo.com");
    array_put(s, "www.facebook.com");
    char *hostname;
    array_get(s, &hostname);
    printf("%s\n", hostname);
    array_get(s, &hostname);
    printf("%s\n", hostname);
    array_get(s, &hostname);
    printf("%s\n", hostname);
    array_free(s);
    free(s);
    return 0;
}