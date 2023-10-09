#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 3

sem_t semaphore;

void* thread_function(void* thread_id) {
    int tid = *((int*)thread_id);
    int sem_value;
    sem_getvalue(&semaphore, &sem_value);
    printf("Thread %d is waiting...\n", tid);
    printf("Thread %d, sem available before acquiring: %d \n", tid, sem_value);
    sem_wait(&semaphore); // Wait (decrement) the semaphore
    printf("Thread %d has acquired the semaphore!\n", tid);
    // Critical section: Access shared resources
    sem_getvalue(&semaphore, &sem_value);
    printf("Thread %d, sem available after acquiring: %d \n", tid, sem_value);
    sleep(20);
    printf("Thread %d is releasing the semaphore...\n", tid);
    sem_post(&semaphore); // Signal (increment) the semaphore
    sem_getvalue(&semaphore, &sem_value);
    printf("Thread %d, sem available after releasing: %d \n", tid, sem_value);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    //sem_init(&semaphore, 0, 1); // 1 = mutex
    sem_init(&semaphore, 0, 2); // 1 = mutex

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_function, (void*)&thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore); // Destroy the semaphore

    return 0;
}
