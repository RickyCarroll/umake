/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 */



#include "arg_parse.h"

/* CONSTANTS */

/* PROTOTYPES */

/* GLOBALS */


/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void   processline(char* line);

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
  int argcp = 0;
  char** args = arg_parse(line, &argcp);
  if (argcp == 1){
    free (args);
    return;
  }
  const pid_t cpid = fork();
  switch(cpid) {
    
  case -1: {
    perror("fork");
    free(args);
    break;
  }
    
  case 0: {
    
    execvp(args[0], args);
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
    free(args);
    break;
  }
  }
}

