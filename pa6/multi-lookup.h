#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H
#include "array.h"
#include "util.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/sem.h>
#include<pthread.h>
#include<stdlib.h>
#include<semaphore.h>
#include<sys/time.h>
#include<pthread.h>

// PA6 definitions
#define MAX_INPUT_FILES 100             // Maximum number of hostname file arguments allowed
#define MAX_REQUESTER_THREADS 10        //- Maximum number of concurrent requester threads allowed
#define MAX_RESOLVER_THREADS 10         //- Maximum number of concurrent resolver threads allowed
#define MAX_IP_LENGTH INET6_ADDRSTRLEN  // INET6_ADDRSTRLEN is the maximum size IP address string util.cs

#define THREAD_NUM MAX_REQUESTER_THREADS + MAX_RESOLVER_THREADS

typedef struct Requester{
    char** input_files;                 
    FILE* serviced_log;                
    pthread_mutex_t file_position_lock; 
    pthread_mutex_t serviced_lock;      
    array* buffer;                     
    int idx;                            
    int file_count;                     
} Requester;

typedef struct Resolver{
    pthread_mutex_t results_lock;
    array* buffer;
    FILE* results_log;                 
} Resolver;

void* requesterThread(void* inputfile);

void* resolverThread(void* outputfile);

#endif