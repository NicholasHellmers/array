#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

```This is a first in last out approach to the bounded buffer problem.```

*/

/*

"array.h"

typedef struct {
    char *buf[ARRAY_SIZE][MAX_NAME_LENGTH];          // the buffer
    int count;                      // number of elements in the buffer
} array;


pthread_mutex_t mutex;          // mutual exclusion
sem_t empty;                    // empty
sem_t full;                     // full

int  array_init(array *s);                   // initialize the array
int  array_put (array *s, char *hostname);   // place element into the array, block when full
int  array_get (array *s, char **hostname);  // remove element from the array, block when empty
void array_free(array *s);                   // free the array's resources

*/


int array_init(array *s) {                  // initialize the array
    sem_init(&empty, 0, ARRAY_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    s->count = 0;
    return 0;
}
int array_put (array *s, char *hostname) {  // place element into the array, block when full
    while(1) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        strcpy(s->buffer[s->count], hostname);
        s->count++;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    pthread_exit(0);
}
int array_get (array *s, char **hostname) { // remove element from the array, block when empty
    while(1) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        s->count--;
        strcpy(*hostname, s->buffer[s->count]);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    pthread_exit(0);
}
    
void array_free(array *s) {                 // free the array's resources
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
}

// int main() {
//     array *s;
//     array_init(s);
//     array_put(s, "www.google.com");
//     array_put(s, "www.facebook.com");
//     array_put(s, "www.twitter.com");

//     char *hostname;
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);
//     array_get(s, &hostname);
//     printf("%s\n", hostname);

//     array_free(s);
//     return 0;
// }