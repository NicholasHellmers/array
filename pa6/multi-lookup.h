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
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

#define THREAD_NUM MAX_REQUESTER_THREADS + MAX_RESOLVER_THREADS

typedef struct Requester {
    char** input_files;
    FILE* s_log;
    pthread_mutex_t fp_lock, s_lock;
    array* buffer;
    int idx, file_count;
} Requester;

typedef struct Resolver {
    pthread_mutex_t r_lock;
    array* buffer;
    FILE* r_log;
} Resolver;

void* requesterThread(void* inputfile);

void* resolverThread(void* outputfile);

#endif