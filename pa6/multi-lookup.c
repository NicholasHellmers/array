#include "multi-lookup.h"

#define SBUFSIZE 1024
#define INPUTFS "%1024s"

pthread_mutex_t bufferLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCountLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outputLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t inputLock = PTHREAD_MUTEX_INITIALIZER;

array s;

FILE* inputfp = NULL;
FILE* outputfp = NULL;
int requestThreadCount;

void* requesterTread(void* inputfile) {
    char hostname[SBUFSIZE];
    char errorstr[SBUFSIZE];

    pthread_mutex_lock(&inputLock);

    inputfp = fopen(inputfile, "r");

    if(!inputfp) {
        fprintf(errorstr, "Error when opening the input file.");
        perror(errorstr);
        exit(EXIT_FAILURE);
    }

    while (fscanf(inputfp, INPUTFS, hostname) > 0) {
        pthread_mutex_lock(&bufferLock);
        while(queue_is_full(&s)){
            pthread_mutex_unlock(&bufferLock);
            // sleep between 0 and 100 micro seconds
            usleep((rand()%100)*1000);
            pthread_mutex_lock(&bufferLock);
        }
        /* strdup mallocs memory and copys the argument into this new memory,
        need to free this in resolver or else get memory leaks */
        queue_push(&s, strdup(hostname));
        pthread_mutex_unlock(&bufferLock);
    }

    fclose(inputfp);
    pthread_mutex_unlock(&inputLock);

    pthread_mutex_lock(&threadCountLock);
    requestThreadCount--;
    pthread_mutex_unlock(&threadCountLock);
    }

    return EXIT_SUCCESS;
}

void* resolverThread(void* outputfile) {
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    
    if(argc < 4) {
        fprintf(stderr, "Not enough args: %d \n", (argc - 1));
    }

    pthread_t threads[argc-2];
    pthread_t rthreads[5];

    array_init(&s);

    requestThreadCount = argc - 2;
    
    fclose(fopen(argv[argc-1], "w"));

    for(int t = 0; t < argc - 2; t++) {
        if (pthread_create(&(threads[t]), NULL, requesterTread, argv[argc-(t+2)])){
            printf("ERROR when creating requester threads");
            exit(EXIT_FAILURE);
        }
    }

    for(int t = 0; t < 5; t++) {
        /* create 5 resolver threads, print if error */
        if (pthread_create(&(rthreads[t]), NULL, resolverThread, argv[argc-1])){
            printf("ERROR when creating resolver threads");
            exit(EXIT_FAILURE);
        }
    }

    /* wait for all requester theads to finish */
    for(int t = 0; t < argc-2; t++){
        pthread_join(threads[t],NULL);
    }

    /* wait for all resolver threads to finish */
    for(int t = 0; t < 5; t++){
        pthread_join(rthreads[t],NULL);
    }

    /* free array from memory */
    array_free(&s);

    return EXIT_SUCCESS;

}