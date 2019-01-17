/*
 *  NAME:   ALISSA NIEWIADOMSKI
 *  EMAIL:  aniewiadomski@ucla.edu
 *  UID:    *********
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

#define STDIN 0
#define STDOUT 1

/*
 * Status codes
 */
#define ERROR_INPUT_FILE 2
#define ERROR_OUTPUT_FILE 3
#define UNRECOGNIZED_ARGUMENT 1
#define CATCH_SEGFAULT 4
#define DUMPCORE 139

void causeSegFault(void);
void catchHandler(int signal);
void copyInputToOutput(void);

int main(int argc, char **argv) {
  /*****************
   * INITIAL SETUP *
   *****************/
  int option;
  char optString[] = "i:o:cds";
    
  /*
   *  The following boolean is a flag for whether or not segmentation faults
   *  should catch or dumpcore.
   */
  bool shouldDumpcore = true;
    
  /*
   * Input filenames, should they be specified
   */
  char* inputFilename = NULL;
  char* outputFilename = NULL;

    
  // Specifies possible command line arguments
  static struct option long_options[] = {
    { "input",     required_argument, 0, 'i' },
    { "output",    required_argument, 0, 'o' },
    { "segfault",  no_argument,       0, 's' },
    { "dump-core", no_argument,       0, 'd' },
    { "catch",     no_argument,       0, 'c' },
    { 0,           0,                 0,  0  }
  };
    
  /****************
   * END OF SETUP *
   ****************/


  /***************************
   * BEGIN PARSING ARGUMENTS *
   ***************************/
  while (1) {
    /*
     * getopt_long gets the current argument
     */
    int optionIdx = 0;
    option = getopt_long(argc, argv, optString, long_options, &optionIdx);
   
    
    // If we're at the end of the options, exit the while loop
    if (option == -1){
      break;
    }
        
    switch (option) {
      /*
       * --input=filename
       */
    case 'i':
      inputFilename = optarg;
      break;
      /*
       * --output=filename
       */
    case 'o':
      // printf("At option o with value '%s'\n", optarg);
      outputFilename = optarg;
      break;
      /*
       * --segfault
       */
    case 's':
      // printf("At option s\n");
      break;
      /*
       * --dump-core
       */
    case 'd':
      // printf("At option d\n");
      break;
      /*
       * --catch
       */
    case 'c':
      // printf("At option c\n");
      break;
            
      /*
       * Invalid argument
       */
    case '?':
      fprintf(stderr, "INVALID ARGUMENT.\n");
      fprintf(stderr, "Usage: ./lab0 [--input=filename] [--output=filename] [--dump-core] [--segfault] [--catch]\n");
      exit(UNRECOGNIZED_ARGUMENT);
    }
  }
  /*************************
   * END PARSING ARGUMENTS *
   *************************/
    
    
  /*************************
   * BEGIN EXECUTING TASKS *
   *************************/
  /*
   * Process arguments in order--important for dump-core, catch, and segfault
   */
  optind = 1;

  while (1) {
    /*
     * getopt_long gets the current argument
     */
    int optionIdx1 = 0;
    option = getopt_long(argc, argv, optString, long_options, &optionIdx1);
   
    // If we're at the end of the options, exit the while loop
    if (option == -1) {
      break;
    }
        
    switch (option) {
      /*
       * --input=filename
       */
    case 'i':
      break;
      /*
       * --output=filename
       */
    case 'o':
      break;
      /*
       * --segfault
       */
    case 's':
      if (!shouldDumpcore) {
	signal(SIGSEGV, catchHandler); 
      }
      causeSegFault();
      break;
      /*
       * --dump-core
       */
    case 'd':
      shouldDumpcore = true;
      break;
      /*
       * --catch
       */
    case 'c':
      shouldDumpcore = false;
      break;
                
    default:
      break;
    }
  }

    
  if (inputFilename) {
    int ifd = open(inputFilename, O_RDONLY);
    /*
     * Close existing STDIN, dup ifd to STDIN, and close temp ifd
     */
    if (ifd >= 0) {
      close(0);
      dup(ifd);
      close(ifd);
    } else {
      fprintf(stderr, "Error opening file %s.\nError: %d. Message: %s \n", inputFilename, errno, strerror(errno));
      exit(ERROR_INPUT_FILE);
    }
  }
    
  if (outputFilename) {
    int ofd = creat(outputFilename, 0666);
    /*
     * Close existing STDOUT, dup ofd to STDOUT, and close temp ofd
     */
    if (ofd >= 0) {
      close(1);
      dup(ofd);
      close(ofd);
    } else {
      fprintf(stderr, "Error creating output file %s.\nError: %d. Message: %s \n", outputFilename, errno, strerror(errno));
      exit(ERROR_OUTPUT_FILE);
    }
  }
    
  copyInputToOutput();
  
  exit(0);
}



/*******************
 * HELPER FUNCTIONS*
 *******************/

void causeSegFault() {
  char *ptr = NULL;
  *ptr = 1;
}

void catchHandler(int signal) {
  if (signal == SIGSEGV) {
    fprintf(stderr, "A segmentation fault was caught with the signal handler.\n");
    exit(CATCH_SEGFAULT);
  }
}

void copyInputToOutput() {
  char *buff = malloc(sizeof(char));
 
  /*
   * Keep going until it reaches an error or EOF
   */
  while (read(STDIN, buff, sizeof(char)) > 0) {
    if (write(STDOUT, buff, 1) < 0) {
      fprintf(stderr, "Error writing to stdout:%s\n", strerror(errno));
      exit(ERROR_OUTPUT_FILE);
    }
  }
    
  /*
   * If problem reading from input
   */
  if (read(STDIN, buff, sizeof(char)) < 0) {
    fprintf(stderr, "Error reading from stdin:%s\n", strerror(errno));
    exit(ERROR_INPUT_FILE);
  }
    
  free(buff);
}
