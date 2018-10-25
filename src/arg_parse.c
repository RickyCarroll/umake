#include "arg_parse.h"

/* countargs
 * line - the command line to execute from uMakefile
 * Eats white space and counts the first character of each arg
 */
static int countargs(char* line);

char** arg_parse(char* line, int* argcp){
  *argcp = countargs(line)+1;
  char** args = malloc((*argcp)*sizeof(char*));
  bool first = true;
  int lcount = 0;
  int acount = 0;
  while (linelen > lcount){
    while (!isspace(line[lcount])){
      if (first){
	args[acount] = &line[lcount];
	first = false;
	acount++;
      }
      lcount++;
    }
    line[lcount] = '\0';
    lcount++;
    first = true;
  }
  args[(*argcp)-1] = NULL;
  return args;
}

static int countargs(char* line){  
  char* count = line;
  int ix = 0;
  int numargs = 0;
  int begword = 1;
  while(ix < linelen || count[ix] != '\0'){
    if(isspace(count[ix])){
      begword = 1;
      ix++;
    }
    else if (count[ix] != '\0' && begword){
      numargs++;
      begword = 0;
    }
    else if(!isspace(count[ix])){
      ix++;
    }
  }
  return numargs;
}
