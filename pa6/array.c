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

int array_init(array *s) {                      // initialize the array
    pthread_mutex_init(&s->M, NULL);       
    pthread_cond_init(&s->DC, NULL);               /* Initialize consumer condition variable */
    pthread_cond_init(&s->DP, NULL);               /* Initialize producer condition variable */
    s->count = 0;
    return 0;
}

void* array_put (array *s, char *hostname) {
    pthread_mutex_lock(&s->M);	                    /* protect buffer */
    while (s->count == ARRAY_SIZE)		        /* If there is something in the buffer then wait */
        pthread_cond_wait(&s->DP, &s->M);

    strcpy(s->buffer[s->count], hostname);
    s->count++;
    // printf("Producer wrote %s\n", hostname);
    pthread_cond_signal(&s->DC);	                /* wake up consumer */
    pthread_mutex_unlock(&s->M);	                /* release the buffer */
    // pthread_exit(0);
    return NULL;
}

void* array_get (array *s, char **hostname) {
    pthread_mutex_lock(&s->M);	                    /* protect buffer */
    while (s->count == 0)			            /* If there is nothing in the buffer then wait */
        pthread_cond_wait(&s->DC, &s->M);
    s->count--;
    strcpy(*hostname, s->buffer[s->count]);
    // printf("Consumer reads %s\n", *hostname);
    pthread_cond_signal(&s->DP);	                /* wake up producer */
    pthread_mutex_unlock(&s->M);	                /* release the buffer */
    // pthread_exit(0);
    return NULL;
}

void array_free(array *s) {                     // free the array's resources
    pthread_mutex_destroy(&s->M);	                /* Free up M mutex */
    pthread_cond_destroy(&s->DC);	                /* Free up consumer condition variable */
    pthread_cond_destroy(&s->DP);	                /* Free up producer condition variable */
}