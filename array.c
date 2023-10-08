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
    char *buf[ARRAY_SIZE];          // the buffer
    sem_t mutex = 1;                // mutual exclusion
    sem_t empty = ARRAY_SIZE;       // empty
    sem_t full = 0;                 // full
    int count = 0;                  // number of elements in the buffer
} array;

*/

int array_init(array *s) {                  // initialize the array
    sem_open(&s->mutex, 0, 1);
    sem_open(&s->empty, 0, ARRAY_SIZE);
    sem_open(&s->full, 0, 0);
    s->count = 0;
    return 0;
}
int array_put (array *s, char *hostname) {  // place element into the array, block when full
    while (s->count == ARRAY_SIZE) {
        sem_wait(&s->full);
    }
    sem_wait(&s->mutex);
    s->buf[s->count] = hostname;
    s->count++;
    sem_post(&s->mutex);
    sem_post(&s->empty);
    return 0;
}
int array_get (array *s, char **hostname) { // remove element from the array, block when empty
    while (s->count == 0) {
        sem_wait(&s->empty);
    }
    sem_wait(&s->mutex);
    *hostname = s->buf[s->count];
    s->count--;
    sem_post(&s->mutex);
    sem_post(&s->full);
    return 0;
}
void array_free(array *s) {                 // free the array's resources
    sem_close(&s->mutex);
    sem_close(&s->empty);
    sem_close(&s->full);
}

// int main() {
//     array *s;
//     array_init(s);
//     array_put(s, "www.google.com");
//     array_put(s, "www.facebook.com");
//     array_put(s, "www.twitter.com");
//     array_put(s, "www.youtube.com");
//     array_put(s, "www.instagram.com");
//     array_put(s, "www.reddit.com");
//     array_put(s, "www.linkedin.com");
//     array_put(s, "www.pinterest.com");
//     char *hostname;
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_free(s);
//     return 0;
// }