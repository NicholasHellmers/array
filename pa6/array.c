/*
 *  Solution to Producer Consumer Problem
 *  Using Ptheads, a mutex and condition variables
 */

/*
    In this version the buffer is a single number.
    The producer is putting numbers into the shared buffer
    (in this case sequentially)
    And the consumer is taking them out.
    If the buffer contains zero, that indicates that the buffer is empty.
    Any other value is valid.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"

int array_init(array *s) {                  // initialize the array
    pthread_mutex_init(&M, NULL);
    pthread_cond_init(&DC, NULL); /* Initialize consumer condition variable */
    pthread_cond_init(&DP, NULL); /* Initialize producer condition variable */
    s->count = 0;
    return 0;
}

int array_put (array *s, char *hostname) {
    pthread_mutex_lock(&M);	/* protect buffer */
    while (s->count == ARRAY_SIZE)		       /* If there is something in the buffer then wait */
        pthread_cond_wait(&DP, &M);

    strcpy(s->buffer[s->count], hostname);
    s->count++;
    printf("Producer wrote %d\n", hostname);
    pthread_cond_signal(&DC);	/* wake up consumer */
    pthread_mutex_unlock(&M);	/* release the buffer */
    pthread_exit(0);
}

int array_get (array *s, char **hostname) {
    pthread_mutex_lock(&M);	/* protect buffer */
    while (s->count == 0)			/* If there is nothing in the buffer then wait */
        pthread_cond_wait(&DC, &M);
    s->count--;
    strcpy(*hostname, s->buffer[s->count]);
    printf("Consumer reads %d\n", *hostname);
    pthread_cond_signal(&DP);	/* wake up producer */
    pthread_mutex_unlock(&M);	/* release the buffer */
    pthread_exit(0);
}

void array_free(array *s) {                 // free the array's resources
    pthread_mutex_destroy(&M);	/* Free up M mutex */
    pthread_cond_destroy(&DC);	/* Free up consumer condition variable */
    pthread_cond_destroy(&DP);	/* Free up producer condition variable */
}