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

#define MAX_INPUT_FILES 100             // Maximum number of hostname file arguments allowed
#define MAX_REQUESTER_THREADS 10        //- Maximum number of concurrent requester threads allowed
#define MAX_RESOLVER_THREADS 10         //- Maximum number of concurrent resolver threads allowed
#define MAX_IP_LENGTH INET6_ADDRSTRLEN  // INET6_ADDRSTRLEN is the maximum size IP address string util.cs

#define THREAD_NUM MAX_REQUESTER_THREADS + MAX_RESOLVER_THREADS

typedef struct Requesters{
    char** input_files;                 //shared access to list input files (names)
    FILE * serviced_log;                //argv[3]
    pthread_mutex_t file_position_lock; //provide mutual exclusion for writing
    pthread_mutex_t serviced_lock;      //writing log
    array * shared_buffer;              //pointer to stack struct
    int global_index;                   // the key to multithreaded file reads
    int file_count;                     // argc-5
}Requesters;

typedef struct Resolvers{
    pthread_mutex_t results_lock;       //provide mutual exclusion for writing to results.txt
    array * shared_buffer;
    FILE * results_log;                 //argv[4]
}Resolvers;

void* requesterTread(void* inputfile);

void* resolverThread(void* outputfile);

#endif