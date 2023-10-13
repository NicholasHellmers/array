#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

#define THREAD_NUM 10

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
        printf("PUT: %s\n", s->buffer[s->count]);
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
        printf("GOT: %s\n", *hostname);
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

int main() {
    pthread_t th[THREAD_NUM];
    array *s;
    array_init(s);
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (i > 0) {
            if (pthread_create(&th[i], NULL, &array_put, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &array_get, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    array_free(s);
    return 0;
}