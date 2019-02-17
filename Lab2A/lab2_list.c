// ALISSA NIEWIADOMSKI
// *********
// aniewiadomski@ucla.edu

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "SortedList.h"
#include <errno.h>
#include <signal.h>

#define RET_ERROR_BAD_ARGS          1
#define RET_ERROR_SYSCALL_FAIL      1
#define RET_ERROR_OTHER             2
#define RET_EXIT_SUCCESS            0

#define SPIN_LOCK_LOCKED            1
#define SPIN_LOCK_UNLOCKED          0

#define TEST_NAME_DEFAULT           "list-"

#define INCREMENT_VAL               1


// Globals
char charset[] = "0123456789"
"abcdefghijklmnopqrstuvwxyz"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

SortedList_t* head;
SortedListElement_t* nodes;

int numThreads = 1;             // default
int numIterations = 1;          // default

/*
 * Define the types of locks that we will use, none by default
 */
typedef enum l {
    none,
    mutex,
    spin,
} lock;

// Default
lock lock_used = none;
// For mutex lock
pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
// For spin lock
int s_lock = SPIN_LOCK_UNLOCKED;
// Option to yield
int opt_yield = 0;

/*
 * Handles any segmentation faults.
 */
void segfaultHandler() {
    fprintf(stderr, "SEGMENTATION FAULT.\n");
    exit(RET_ERROR_SYSCALL_FAIL);
}


/*
 * Sets dest to a random string for keys of the nodes
 */
void randString(char *dest, size_t length) {    
    while (length-- > 0) {
        size_t index = (rand() % (sizeof charset - 1)) + 1;
        *dest++ = charset[index];
    }
    *dest = '\0';
}


/*
 * Helper function that performs list operations on our linked list
 */
void* list_wrapper(void *arguments) {
    if (lock_used == mutex) {
        pthread_mutex_lock(&m_lock);
    } else if (lock_used == spin) {
        while (__sync_lock_test_and_set(&s_lock, SPIN_LOCK_LOCKED) == SPIN_LOCK_LOCKED);
    }
    
    SortedListElement_t* thread_localNodes = arguments;

    long i;
    for (i = 0; i < numIterations; i++) {
        SortedList_insert(head, (SortedListElement_t *) (thread_localNodes + i));
    }
    
    long length;
    length = SortedList_length(head);
    
    long j;
    for (j = 0; j < numIterations; j++) {
        SortedListElement_t* lookup = SortedList_lookup(head, (thread_localNodes + j)->key);
        SortedList_delete(lookup);
    }
    
    if (lock_used == mutex) {
        pthread_mutex_unlock(&m_lock);
    } else if (lock_used == spin) {
       __sync_lock_release(&s_lock);
    }
    return 0;
}


/*
 * Driver program
 */

int main(int argc, char *argv[]) {
    srand(time(0));
    
    int option;
    struct timespec startTime, endTime;
    
    static struct option long_options[] = {
        { "threads",    required_argument, 0, 't' },
        { "iterations", required_argument, 0, 'i' },
        { "yield",      required_argument, 0, 'y' },
        { "sync",       required_argument, 0, 's' },
        { 0,            0,                 0,  0  }
    };
    
    signal(SIGSEGV, segfaultHandler);
    
    // Parse arguments
    while (1) {
        int optionIdx = 0;
        size_t i;
	       
        option = getopt_long(argc, argv, "", long_options, &optionIdx);
        
        // End of parsing arguments
        if (option == -1){
            break;
        }
        
        switch(option) {
            case 't':
                numThreads = atoi(optarg);
                break;
                
            case 'i':
                numIterations = atoi(optarg);
                break;
                
            case 's':
                if (strcmp("s", optarg) && strcmp("m", optarg)) {
                    fprintf(stderr, "Please supply a valid argument to --sync.\n");
                    fprintf(stderr, "Usage: ./lab2_add [--threads=nThreads] [--sync=lock] [--yield] [--iterations=nIterations]\n");
                    exit(RET_ERROR_BAD_ARGS);
                }
                
                // Look at the optarg to determine the type of lock.
                if (!strcmp("s", optarg)) {
                    lock_used = spin;
                } else {
                    lock_used = mutex;
                }
                break;
                
            case 'y':
                for (i = 0; i < strlen(optarg); i++) {
                    char c = optarg[i];
                    
                    if (c != 'l' && c != 'd' && c != 'i') {
                        fprintf(stderr, "Invalid argument to --yield\n");
                        fprintf(stderr, "Usage: ./lab2_list [--threads=nThreads] [--sync=[m|s]] [--yield=[idl]] [--iterations=nIterations]\n");
                        exit(RET_ERROR_BAD_ARGS);
                        
                    }
                    
                    if (c == 'l') {
                        opt_yield |= LOOKUP_YIELD;
                    } else if (c == 'd') {
                        opt_yield |= DELETE_YIELD;
                    } else {
                        opt_yield |= INSERT_YIELD;
                    }
                }
                break;
                
            case '?':
                fprintf(stderr, "INVALID ARGUMENT\n");
                fprintf(stderr, "Usage: ./lab2_list [--threads=nThreads] [--sync=[m|s]] [--yield=[idl]] [--iterations=nIterations]\n");
                exit(RET_ERROR_BAD_ARGS);
        }
    }
    
    
    if (numThreads <= 0) {
        fprintf(stderr, "Invalid number of threads.\n");
        exit(RET_ERROR_OTHER);
    }
    
    // Create head and the random elements
    head = malloc(sizeof(SortedList_t));
    head->key = NULL;
    head->next = NULL;
    head->prev = NULL;
    
    nodes = malloc(numThreads * numIterations * sizeof(SortedListElement_t));
    char** randKeys = malloc(numThreads * numIterations * sizeof(char*));
 
    int x;
    for (x = 0; x < numThreads * numIterations; x++) {
        int length = (rand() % (32 - 2)) + 2;
        char str[length];
        randKeys[x] = malloc(sizeof(char) * length);
        
        randString(str, sizeof str - 1);
        strcpy(randKeys[x], str);
        (nodes + x)->key = randKeys[x];
    }

    
    // Create an array of threads
    pthread_t *threads = malloc(sizeof(pthread_t) * numThreads);
    if (threads == NULL) {
        fprintf(stderr, "There was not enough memory to allocate %d threads.\n", numThreads);
        exit(RET_ERROR_OTHER);
    }
    
    // Get our starting time for this run
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);
    
    // Execute each thread
    int i;
    for (i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, &list_wrapper, (void *)(nodes + numIterations * i));
    }

    // Wait for threads to finish
    int j;
    for(j = 0; j < numThreads; j++) {
        pthread_join(threads[j], NULL);
    }

    // Get our ending time for this run
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);

    // Get ending length
    int endLength = SortedList_length(head);
    // Free our allocated memory
    free(head);
    free(nodes);
    free(randKeys);
    
    // Check that we finished with a list length of 0
    if (endLength) {
        fprintf(stderr, "The list did not have an empty length at the end of the runs.\n");
        exit(RET_ERROR_OTHER);
    }
    
    // Print results
    long numOperations = numThreads * numIterations * 3;
    long totalRunTime = endTime.tv_nsec - startTime.tv_nsec;

    // Print test names
    printf("list-");
    switch(opt_yield) {
        case 0:
            printf("none");
            break;
        case 1:
            printf("i");
            break;
        case 2:
            printf("d");
            break;
        case 3:
            printf("id");
            break;
        case 4:
            printf("l");
            break;
        case 5:
            printf("il");
            break;
        case 6:
            printf("dl");
            break;
        case 7:
            printf("idl");
            break;
        default:
            break;
    }
    
    if (lock_used == spin) {
        printf("-s");
    } else if (lock_used == mutex) {
        printf("-m");
    } else {
        printf("-none");
    }
    
    
    printf(",%d,%d,%d,%ld,%ld,%ld\n", numThreads, numIterations, 1, numOperations, totalRunTime, totalRunTime / numOperations);

    return RET_EXIT_SUCCESS;
}

