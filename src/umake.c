/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>

/* CONSTANTS */

/* PROTOTYPES */

/* GLOBALS */
ssize_t linelen;

/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void   processline(char* line);

/* Arg parse
 * line   The command line to process
 */

char** arg_parse(char* line);


/* Count args
 * line   The command line count number of args
 * This function counts the number of arguments in the line.
 */
int    countargs(char* line);

/* Main entry point.
 * argc    A count of command-line arguments 
 * argv    The com mand-line argument valus
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time.  Lines with a leading tab
 * character ('\t') are interpreted as a command and passed to processline minus
 * the leading tab.
 */
int main(int argc, const char* argv[]) {
  
  FILE* makefile = fopen("./uMakefile", "r");
  size_t  bufsize = 0;
  char*   line    = NULL;
  linelen = getline(&line, &bufsize, makefile);
  
  while(-1 != linelen) {

    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }

    if(line[0] == '\t') 
      processline(&line[1]);

    linelen = getline(&line, &bufsize, makefile);
  }

  free(line);
  return EXIT_SUCCESS;
}


/* Process Line
 * 
 */
void processline (char* line) {

  char** args = arg_parse(line);
  const pid_t cpid = fork();
  switch(cpid) {
    
  case -1: {
    perror("fork");
    break;
  }
    
  case 0: {
    
    execvp(args[0], args);
    /*
      execlp(line, line, (char*)(0));
    */
    perror("execvp");
    exit(EXIT_FAILURE);
    break;
  }
    
  default: {
    int   status;
    const pid_t pid = wait(&status);
    if(-1 == pid) {
      perror("wait");
    }
    else if (pid != cpid) {
      fprintf(stderr, "wait: expected process %d, but waited for process %d",
              cpid, pid);
    }
    break;
  }
  }
}

char** arg_parse(char* line){
  int numargs = countargs(line) + 1;
  char** args = malloc(numargs*sizeof(char*));
  
  bool first = true;
  int lcount = 0;
  int acount = numargs;
  while (linelen > lcount){
    //only interested in args
    while (isalnum(line[lcount])){
      if (first){
	args[numargs - acount] = &line[lcount];
	first = false;
	acount--;
      }
      lcount++;
    }
    //replace whitespace with null char
    line[lcount] = '\0';
    lcount++;
    first = true;
  }
  return args;
}

int countargs(char* line){  
  char* count = line;
  int ix = 0;
  int numargs = 0;
  while(count[ix] != '\0'){
    //loop through whitespace
    while(!isalnum(count[ix])){
      ix++;
    }
    //first letter if not NULL
    if (count[ix] != '\0'){
      numargs++;
    }
    //loop through arg
    while(isalnum(count[ix])){
      ix++;
    }
  }
  return numargs;
}

