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

#define RET_ERROR_BAD_ARGS          1
#define RET_ERROR_SYSCALL_FAIL      1
#define RET_ERROR_OTHER             2
#define RET_EXIT_SUCCESS            0

#define SPIN_LOCK_LOCKED            1
#define SPIN_LOCK_UNLOCKED          0

#define TEST_NAME_DEFAULT           "add-none"
#define TEST_NAME_SPIN              "add-s"
#define TEST_NAME_CS                "add-c"
#define TEST_NAME_MUTEX             "add-m"
#define TEST_NAME_YIELD_NONE        "add-yield-none"
#define TEST_NAME_YIELD_SPIN        "add-yield-s"
#define TEST_NAME_YIELD_CS          "add-yield-c"
#define TEST_NAME_YIELD_MUTEX       "add-yield-m"
#define INCREMENT_VAL               1

/*
 * Define the types of locks that we will use, none by default
 */
typedef enum l {
    none,
    mutex,
    spin,
    compare_swap,
} lock;

// Default
lock lock_used = none;
// For mutex lock
pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
// For spin lock
int s_lock = SPIN_LOCK_UNLOCKED;


/*
 * Whether or not we should allow the threads to yield
 */
int opt_yield = 0;

/*
 * Helper struct for our add_wrapper
 * Used in pthread_create to easily pass in multiple function arguments
 */
struct args_to_add {
    long long *pointer;
    long long value;
    int iterations;
};


/*
 * Simple add module that increments *pointer by value
 */
void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield) {
        sched_yield();
    }
    *pointer = sum;
}

/*
 * Add module wrapped by mutex lock
 */
void mutexWrapper_add(long long *pointer, long long value) {
    pthread_mutex_lock(&m_lock);
    add(pointer, value);
    pthread_mutex_unlock(&m_lock);
}

/*
 * Add module wrapped by spin lock
 */
void spinWrapper_add(long long *pointer, long long value) {
    while(__sync_lock_test_and_set(&s_lock, SPIN_LOCK_LOCKED) == SPIN_LOCK_LOCKED)
        continue;
    add(pointer, value);
    __sync_lock_release(&s_lock);
}

/*
 * Add module wrapped by compare and swap lock
 */
void compswapWrapper_add(long long *pointer, long long value) {
    // Don't call the add module, need to wrap the compare and swap with the do while loop.
    long long exp;      // The expected value that will be the compare to value.
    long long s;        // Add the expected (*pointer) and the current value
                        // Same as adding *pointer + value, but using compare and swap
                        // to set the value of pointer to this.
    do {
        // Do this once to acquire the lock
        exp = *pointer;
        s = exp + value;
        if (opt_yield) {
            sched_yield();
        }
    } while(exp != __sync_val_compare_and_swap(pointer, exp, s));
}

/*
 * Gets the test name based on variables.
 */
char* getTestName() {
    // If yielding
    if (opt_yield) {
        if (lock_used == none) {
            return TEST_NAME_YIELD_NONE;
        } else if (lock_used == mutex) {
            return TEST_NAME_YIELD_MUTEX;
        } else if (lock_used == spin) {
            return TEST_NAME_YIELD_SPIN;
        }
        return TEST_NAME_YIELD_CS;
    }
    // Else--no yielding
    if (lock_used == mutex) {
        return TEST_NAME_MUTEX;
    } else if (lock_used == spin) {
        return TEST_NAME_SPIN;
    } else if (lock_used == compare_swap) {
        return TEST_NAME_CS;
    }
    return TEST_NAME_DEFAULT;
}


/*
 * Helper function that calls our basic add module
 * the specified number of times, using -1 and 1 as values
 */
void* add_wrapper(void *arguments) {
    struct args_to_add *args = arguments;
    
    // Add 1
    int i;
    for (i = 0; i < args->iterations; i++) {
        if (lock_used == mutex) {
            mutexWrapper_add(args->pointer, args->value);
        } else if (lock_used == spin) {
            spinWrapper_add(args->pointer, args->value);
        } else if (lock_used == compare_swap) {
            compswapWrapper_add(args->pointer, args->value);
        } else {
            add(args->pointer, args->value);
        }
    }
    
    // Add -1
    int j;
    for (j = 0; j < args->iterations; j++) {
        if (lock_used == mutex) {
            mutexWrapper_add(args->pointer, -(args->value));
        } else if (lock_used == spin) {
            spinWrapper_add(args->pointer, -(args->value));
        } else if (lock_used == compare_swap) {
            compswapWrapper_add(args->pointer, -(args->value));
        } else {
            add(args->pointer, -(args->value));
        }
    }
    
    return 0;
}


/*
 * Driver program
 */
int main(int argc, char *argv[]) {
    int option;
    int numThreads = 1;             // default
    int numIterations = 1;          // default
    
    struct timespec startTime, endTime;
    
    static struct option long_options[] = {
        { "threads",    required_argument, 0, 't' },
        { "iterations", required_argument, 0, 'i' },
        { "yield",      no_argument,       0, 'y' },
        { "sync",       required_argument, 0, 's' },
        { 0,            0,                 0,  0  }
    };
    
    // Parse arguments
    while (1) {
        int optionIdx = 0;
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
                
            case 'y':
                opt_yield = 1;
                break;
                
            case 's':
                if (strcmp("s", optarg) && strcmp("m", optarg) && strcmp("c", optarg)) {
                    fprintf(stderr, "Please supply a valid argument to --sync.\n");
                    fprintf(stderr, "Usage: ./lab2_add [--threads=nThreads] [--sync=lock] [--yield] [--iterations=nIterations]\n");
                    exit(RET_ERROR_BAD_ARGS);
                }
                
                // Look at the optarg to determine the type of lock.
                if (!strcmp("s", optarg)) {
                    lock_used = spin;
                } else if (!strcmp("m", optarg)) {
                    lock_used = mutex;
                } else {
                    lock_used = compare_swap;
                }
                break;
                
            case '?':
                fprintf(stderr, "INVALID ARGUMENT\n");
                fprintf(stderr, "Usage: ./lab2_add [--threads=nThreads] [--sync=lock] [--yield] [--iterations=nIterations]\n");
                exit(RET_ERROR_BAD_ARGS);
        }
    }
    
    
    if (numThreads <= 0) {
        fprintf(stderr, "Invalid number of threads.\n");
        exit(RET_ERROR_OTHER);
    }
    
    // Create an array of threads
    pthread_t *threads = malloc(sizeof(pthread_t) * numThreads);
    if (threads == NULL) {
        fprintf(stderr, "There was not enough memory to allocate %d threads.\n", numThreads);
        exit(RET_ERROR_OTHER);
    }
    
    // Initialize arguments to add
    long long counter = 0;
    
    struct args_to_add args;
    args.pointer = &counter;
    args.value = INCREMENT_VAL;
    args.iterations = numIterations;
    
    // Get our starting time for this run
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);
    
    // Execute each thread
    int i;
    for (i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, &add_wrapper, (void*)&args);
    }
    
    // Wait for threads to finish
    int j;
    for(j = 0; j < numThreads; j++) {
        pthread_join(threads[j], NULL);
    }
    
    // Get our ending time for this run
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);
    
    
    // Print results
    long numOperations = numThreads * numIterations * 2;
    long totalRunTime = endTime.tv_nsec - startTime.tv_nsec;
    
    printf("%s,%d,%d,%ld,%ld,%ld,%lld\n", getTestName(), numThreads, numIterations, numOperations, totalRunTime, totalRunTime / numOperations, counter);
    
    return RET_EXIT_SUCCESS;
}
