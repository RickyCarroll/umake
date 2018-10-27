/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 */

/*
  if (argcp == 1){
    free (args);
    return;
  }
*/



#include "arg_parse.h"
#include "target.h"
/* CONSTANTS */

/* PROTOTYPES */

/* GLOBALS */


/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void   processline(char* line);

/* Handle Targets
 * nhead - head of the node list
 * This function finds the targets in the list and sends their rules to processline
 */

void handleTargets(int argc, const char** argv, node* nhead);

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
  
  FILE* makefile   = fopen("./uMakefile", "r");
  size_t  bufsize  = 0;
  char*   line     = NULL;
  node*   nhead    = NULL;
  rule*   rhead    = NULL;
  int     argcp    = 0;
  char**  argsp    = NULL;
  linelen = getline(&line, &bufsize, makefile);

  while(-1 != linelen) {

    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }
    if(line[0] == '\t') {
      rhead = list_rule(line, linelen, rhead);
    }
    else{
      if (rhead != NULL){
        nhead = list_target_depend(argcp, argsp, rhead, nhead);
	rhead = NULL;
      }
      char** args = arg_parse(line, &argcp);
      argsp = malloc(sizeof(char)*argcp);
      for (int i=0; i<argcp-1; i++){
	argsp[i] = malloc(sizeof(strlen(args[i])));
	argsp[i] = strdup(args[i]);
      }
    }

    linelen = getline(&line, &bufsize, makefile);
  }
  
  nhead = list_target_depend(argcp, argsp, rhead, nhead);
  rhead = NULL;
  
  handleTargets(argc, argv, nhead);
  
  free(line);
  return EXIT_SUCCESS;
}


/* Process Line
 * line  - taken from the input file
 * 
 */
void processline (char* line) {
  int argcp = 0;
  char** args = arg_parse(line, &argcp);
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

void handleTargets(int argc, const char** argv, node* nhead){  
  for(int i = 1; i < argc; i++){
    rule* iter = list_search(nhead, argv[i]);
    while(iter->next != NULL){
      processline(&iter->rule[1]);
      iter = iter->next;
    }
    processline(&iter->rule[1]);
  }
}
