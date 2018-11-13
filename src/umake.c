/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
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
 * target - string containing the target
 * nhead - head of the node list
 * This function finds the targets in the list and sends their rules to processline
 * Allows for recursive calls based on dependencies
 */

void handleTargets(char* target, node* nhead);

/* Expand
 * orig    The input string that may contain variables to be expanded
 * new     An output buffer that will contain a copy of orig with all 
 *         variables expanded
 * newsize The size of the buffer pointed to by new.
 * returns 1 upon success or 0 upon failure. 
 *
 * Example: "Hello, ${PLACE}" will expand to "Hello, World" when the environment
 * variable PLACE="World". 
 */
int expand(char* orig, char* new, int newsize);


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
  if (makefile == NULL){
    fprintf(stderr, "uMakefile does not exist in this directory\n");
    exit(1);
  }
  int linelen = getline(&line, &bufsize, makefile);

  while(-1 != linelen) {

    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }
    
    if(line[0] == '\t') {
      rhead = list_rule(line, linelen, rhead);
    }
    else if(strchr(line, 61) != NULL){
      char** args = arg_parse(line, &argcp);
      if (argcp > 2){
	setenv(args[0], args[2], 1);
      }
    }
    else{
      if (rhead != NULL){
	nhead = list_target_depend(argcp, argsp, rhead, nhead);
	rhead = NULL;
      }
      char** args = arg_parse(line, &argcp);
      argsp = malloc(sizeof(char*)*argcp);
      for (int i=0; i<argcp-1; i++){
	argsp[i] = malloc(sizeof(char)*strlen(args[i]));
	argsp[i] = strdup(args[i]);
      }
    }

    linelen = getline(&line, &bufsize, makefile);
  }
  
  nhead = list_target_depend(argcp, argsp, rhead, nhead);
  rhead = NULL;
  
  for(int i = 1; i < argc; i++){
    handleTargets(argv[i], nhead);
  }
  free(line);
  list_free_node(nhead);
  free(nhead);
  return EXIT_SUCCESS;
}


/* Process Line
 * line  - taken from the input file
 * 
 */
void processline (char* line) {
  int argcp = 0;
  char** args;
  if(strchr(line, 36) != NULL){
    char new[1024];
    if(expand(line, new, 1024) < 0){
      fprintf(stderr, "expand");
    }
    args = arg_parse(new, &argcp);
    if (argcp == 1){
      free (args);
      return;
    }
  }else{
    args = arg_parse(line, &argcp);
    if (argcp == 1){
      free (args);
      return;
    }
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

/* Handle Targets
 * target - string containing target
 * nhead - head of the node list
 */

void handleTargets(char* target, node* nhead){
    node* node = list_search(nhead, target);
    if (node != NULL){
      for(int j = 0; node->dependencies[j] != NULL; j++){
	handleTargets(node->dependencies[j], nhead);
      }
    }else{
      return;
    }
    rule* rule = node->rules;
    while(rule->next != NULL){
      char* curline = strdup(&rule->rule[1]);
      processline(curline);
      rule = rule->next;
    }
    processline(&rule->rule[1]);
}


/* Expand
 * orig - original line
 * new - new line with all variables expanded
 * newsize - size of new array
 */
int expand(char* orig, char* new, int newsize){
  char* origp = orig;
  int origi = 0;
  int newi = 0;
  int dol = 0;
  int envstart;
  int end;
  int linelen = strlen(orig);
  while(origi < linelen && newi < newsize-1){
    switch (origp[origi]){
      
    case '$': {
	if(origp[origi+1] == '{'){
	  dol++;
	  envstart = origi+2;
	  origi += 2;
	}
	break;
    }

    case '}': {
	if (dol > 0){
	  dol--;
	  int length = origi-envstart;
	  char* env = malloc(sizeof(char)*(length+1));
	  for (int j = 0; j < length; j++){
	    env[j] = origp[envstart];
	    envstart++;
	  }
	  env[length] = '\0';
	  char* var = getenv(env);

	  if(var!=NULL){
	    free(env);
	    while(*var!='\0' && newi < newsize){
	      new[newi] = *var;
	      newi++;
	      var++;
	    }
	    if(*var!='\0' && newi == newsize){
	      fprintf(stderr, "Not enough room to expand");
	      return -1;
	    }
	  }else{
	    printf("%s not found\n", env);
	    free(env);
	  }
	  
	}else{
	  new[newi] = origp[origi];
	  newi++;
	}
	origi++;
	break;
    }
      
    default:{
      if (dol == 0){
	new[newi] = origp[origi];
	origi++;
	newi++;
      }else{
	origi++;
      }
      break;
    }
      
  }

  }
  if (dol > 0){
    fprintf(stderr, "expected }");
    return -1;
  }else{
    new[newi] = '\0';
    return 0;
  }
}
