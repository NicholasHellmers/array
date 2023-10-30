#include "multi-lookup.h"

#define NOT_RESOLVED "NOT_RESOLVED"

void* requesterThread(void* inputfile) {
    
    Requester *data = (Requester *) inputfile;          //cast void pointer to Requesters struct
    unsigned long tid = (unsigned long) pthread_self(); //thread id
    int count = 0;                                      //local count of the number of files serviced, independent for each requester

    while(1) {
        pthread_mutex_lock(&(data->file_position_lock)); 

        if( data->idx >= data->file_count) { //if all files have been processed //data->file_count<=0
            pthread_mutex_unlock(&(data->file_position_lock));
            printf("thread %lu serviced %d files\n", tid, count);
            fflush(stderr);
            return NULL;
        }
        
        pthread_mutex_unlock(&(data->file_position_lock)); //locking file index
        pthread_mutex_lock(&(data->file_position_lock)); 
        
        int idx = data->idx;
        data->idx++;
        pthread_mutex_unlock(&(data->file_position_lock));
        char *filename;
        filename = data->input_files[idx];
        FILE * fd = fopen(filename, "r+"); //only one thread should read into temp, other threadds should read other files

        if (fd != NULL) {
            char hostname[MAX_NAME_LENGTH];
            while ( fgets(hostname, sizeof(hostname), fd) ) {
                // printf("%s\n", hostname);
                if (hostname[0] != '\n') {
                    hostname[strlen(hostname)-1]='\0';
                    array_put(data->buffer, hostname);
                    pthread_mutex_lock(&(data->serviced_lock));
                    fprintf(data->serviced_log, "%s\n", hostname);
                    fflush(stderr);
                    pthread_mutex_unlock(&(data->serviced_lock));
                }
            }
            fclose(fd); //every thread will close its file after reading it
            count++;

        } else {
            fflush(stderr);
            continue;
        }

    }

    return NULL;
}



void* resolverThread(void* outputfile) {
    /*local to the thread*/
    
    struct timeval start, end;
    gettimeofday(&start, NULL);

    Resolver *data = (Resolver *) outputfile;
    unsigned long tid = (unsigned long) pthread_self();
    char * name = malloc(MAX_NAME_LENGTH);
    
    if(name == NULL){
        printf("Invalid hostname\n. Exiting\n");
        exit(-1);
    }

    char * IP = malloc(MAX_IP_LENGTH);

    if(IP == NULL){
        printf("Invalid IP address\n. Exiting\n");
        exit(-1);
    }
    
    int count = 0;
    
    while(1){
    
        // Poison Pill pattern "https://canvas.colorado.edu/courses/93786/external_tools/1711"
        array_get(data->buffer, &name);
        if (strcmp(name, "PP") == 0) {
            gettimeofday(&end, NULL);
            float time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            printf("Thread %lx resolved %d hostnames in %f seconds\n", tid, count, time);
            fflush(stderr);
            free(name);
            free(IP);
            return NULL;
        }

        // if not poison pill, then resolve the hostname
        if ( dnslookup(name, IP, MAX_IP_LENGTH) == 0 ) {    // Valid hostname
            pthread_mutex_lock(&(data->results_lock)); 
            fprintf(data->results_log, "%s, %s\n", name, IP);
            fflush(stderr);
            pthread_mutex_unlock(&(data->results_lock));
            count++;
        } else {                                            // Invalid hostname
            pthread_mutex_lock(&(data->results_lock)); 
            fprintf(data->results_log, "%s, %s\n", name, NOT_RESOLVED);
            fflush(stderr);
            pthread_mutex_unlock(&(data->results_lock));
        }
    }
    return NULL;
}



int main(int argc, char* argv[]) {
    
    //part 1: file input and error handling (Setup)
    struct timeval start, end;
    
    gettimeofday(&start, NULL);
    
    if(argc < 6){
        printf("Error: Not enough arguments\n");
        return -1;
    }
    
    if(argc>MAX_INPUT_FILES + 5){
        printf("Error: Too many arguments\n");
        return -1;
    }
    
    if(strlen(argv[1]) > 2 || strlen(argv[2]) > 2){
        printf("Error: User entered non integer requester or resolver\n");
        return -1;
    }
    
    int num_requesters = atoi(argv[1]);
    int num_resolvers = atoi(argv[2]);
    
    if(num_requesters < 1 || num_resolvers < 1 || num_requesters > 10 || num_resolvers > 10) {
        printf("Error: Invalid thread request\n");
        return -1;
    }

    printf("User wants %d requesters and %d resolvers\n", num_requesters, num_resolvers);

    char *file_list[argc-5]; //length of file array is amount in /input

    // Allocate memory for file_list
    for(int i=0; i<argc-5; i++){
        file_list[i] = malloc (sizeof(char) * MAX_NAME_LENGTH );

    }

    // Check if file exists
    for(int i = 5; i < argc; i++) {
        if (access(argv[i], F_OK) != 0) {
            printf("invalid file %s\n", argv[i]);
        } else {
            strcpy(file_list[i-5], argv[i]);
        }
    }

    array buffer;
    
    array_init(&buffer);
    
    Requester requester_args;
    Resolver resolver_args;

    pthread_t requesters_id[num_requesters];
    pthread_t resolvers_id[num_resolvers];
    
    requester_args.input_files = file_list;
    
    requester_args.file_count = argc-5;
    
    requester_args.idx = 0;
    
    pthread_mutex_init((&requester_args.file_position_lock), 0); 
    pthread_mutex_init((&requester_args.serviced_lock), 0) ;

    requester_args.buffer = &buffer;
    requester_args.serviced_log = fopen(argv[3], "w+");
    int fdreq = access(argv[3], F_OK | W_OK);

    
    if(requester_args.serviced_log == NULL || fdreq!=0){
        printf("Error: Could not open requester_log file\n");
        return -1;
    }
    
    pthread_mutex_init((&resolver_args.results_lock), 0); 
    
    resolver_args.buffer = &buffer;
    resolver_args.results_log = fopen(argv[4], "w+");
    int fdres = access(argv[4], F_OK | W_OK);

    if(resolver_args.results_log == NULL || fdres != 0){
        printf("Error: Could not open resolver_log file\n");
        return -1;
    }

    // Thread Creation
    // Case 1: fail to create requester thread
    for(int i = 0; i < num_requesters; i++){
    
        if(pthread_create(&requesters_id[i], NULL, requesterThread, &requester_args) != 0){
    
            printf("Failed to create requester thread\n");
        
        }

    }

    // Case 2: fail to create resolver thread
    for(int i = 0; i < num_resolvers; i++){
        if(pthread_create(&resolvers_id[i], NULL, resolverThread, &resolver_args) != 0){
            printf("Failed to create resolver thread\n");
        }
    }

    // Case 3: fail to join requester thread
    for(int i = 0; i < num_requesters; i++){
        if(pthread_join(requesters_id[i], NULL) != 0){
            printf("Failed to join requester\n");
        }
    }

    // Apply poison pill pattern
    while (buffer.count != 0);
    for(int i = 0; i < num_resolvers; i++){
        array_put(&buffer, "PP"); 
    }

    // Case 4: fail to join resolver thread
    for(int i = 0; i < num_resolvers; i++){
        if(pthread_join(resolvers_id[i], NULL) != 0){
            printf("Failed to join resolver\n");
        }
    }

    pthread_mutex_destroy(&(requester_args.file_position_lock));
    pthread_mutex_destroy(&(requester_args.serviced_lock));
    pthread_mutex_destroy(&(resolver_args.results_lock));
    
    for(int i = 0; i < argc - 5; i++){
        free(file_list[i]);
    }

    array_free(&buffer);

    fclose(requester_args.serviced_log);
    fclose(resolver_args.results_log);
    
    gettimeofday(&end, NULL);
    float time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("./multi-lookup: total time is %f seconds\n", time);

    return 0;
}