#include "arg_parse.h"

static int countargs(char* line);

char** arg_parse(char* line){
  int numargs = countargs(line) + 1;
  char** args = malloc(numargs*sizeof(char*));
  
  bool first = true;
  int lcount = 0;
  int acount = numargs;
  while (linelen > lcount){
    //only interested in args
    while (!isblank(line[lcount])){
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

static int countargs(char* line){  
  char* count = line;
  int ix = 0;
  int numargs = 0;
  while(ix < linelen){

    //loop through whitespace
    while(isblank(count[ix])){

      ix++;
    }
    //first letter if not NULL
    if (count[ix] != '\0'){
      numargs++;
    }
    //loop through arg
    while(!isblank(count[ix])){
      ix++;
    }

  }

  return numargs;
}
