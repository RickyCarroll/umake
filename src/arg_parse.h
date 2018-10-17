#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>

/* Arg parse
 * line   The command line to process
 */

char** arg_parse(char* line);


ssize_t linelen;
