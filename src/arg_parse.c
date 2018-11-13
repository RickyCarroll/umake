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
  bool pound = false;
  int lcount = 0;
  int acount = 0;
  int linelen = strlen(line);
  while (linelen > lcount){
    if(!pound){
      if (!isspace(line[lcount]) && line[lcount] != '#'){
	if (first){
	  args[acount] = &line[lcount];
	  first = false;
	  acount++;
	}
	lcount++;
      }
      else if (line[lcount] == '#'){
	line[lcount] = '\0';
	pound = true;
      }
      else{
	line[lcount] = '\0';
	lcount++;
	first = true;
      }
    }else{
      line[lcount] = '\0';
      lcount++;
    }
  }
  args[(*argcp)-1] = NULL;
  return args;
}

static int countargs(char* line){  
  char* count = line;
  int ix = 0;
  int numargs = 0;
  int begword = 1;
  int linelen = strlen(line);
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
