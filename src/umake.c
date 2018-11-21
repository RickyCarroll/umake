/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 */

#include "umake.h"

/* Main entry point.
 * argc    A count of command-line arguments 
 * argv    The com mand-line argument valus
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time.  Lines with a leading tab
 * character ('\t') are interpreted as a command and passed to processline minus
 * the leading tab.
 */
int main(int argc, char* argv[]) {
  
  FILE* makefile   = fopen("./uMakefile", "r");
  size_t  bufsize  = 0;
  char*   line     = NULL;
  node*   nhead    = NULL;
  rule*   rhead    = NULL;
  int     argcp    = 0;
  char**  argsp    = NULL;
  char*   loc      = NULL;
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
      loc = strchr(line, 61);
      if (argcp > 2){
	setenv(args[0], args[1], 1);
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
    redirect(args);
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
  struct stat sbt;
  struct stat sbd;
  int exec = 0;
  if (node != NULL){
    if (node->dependencies[0] != NULL){
      for(int j = 0; node->dependencies[j] != NULL; j++){
	handleTargets(node->dependencies[j], nhead);
	stat(node->dependencies[j], &sbd);
	if(stat(target, &sbt) == -1){
	  exec = 1;
	}else{
	  if(difftime(sbd.st_mtime, sbt.st_mtime) > 0){
	    exec = 1;
	  }
	}
      }
      if (exec == 1){
	process_rules(node->rules);
      }
    }
    else{
      process_rules(node->rules);
    }
  }else{
    return;
  }
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
  int linelen = strlen(orig);
  while(origi < linelen && newi < newsize-1){
    switch (origp[origi]){
      
    case '$': {
	if(origp[origi+1] == '{'){
	  dol++;
	  envstart = origi+2;
	  origi += 2;
	}else{
	  origi++;
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

/* Redirect
 * line - current line to be executed
 */

void redirect (char** line){
  int fd;
  int ix;
  while(line[ix] != NULL){
    switch(*line[ix]){
    case '<':
      line[ix] = NULL;
      ix++;
      fd = open(line[ix], O_CREAT | O_RDONLY, 0600);
      if(fd < 0){
	perror("open:");
      }
      dup2(fd, 0);
      close(fd);
      line[ix] = NULL;
      break;

    case '>':
      if (line[ix][1] == '>'){
	line[ix] = NULL;
        ix++;
	fd = open(line[ix], O_APPEND | O_WRONLY | O_CREAT, 0600);
	if(fd < 0){
	  perror("open:");
	}
	dup2(fd, 1);
	close(fd);
	line[ix] = NULL;
      }else{
	line[ix] = NULL;
        ix++;
	fd = open(line[ix], O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if(fd < 0){
	  perror("open:");
	}
	dup2(fd, 1);
	close(fd);
	line[ix] = NULL;
      }
      break;
    }
    ix++;
  }
}
