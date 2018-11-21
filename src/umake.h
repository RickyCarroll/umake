#include <errno.h>
#include <fcntl.h>
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
 * returns 1 upon success or 0 upon failure. 
 *
 * Example: "Hello, ${PLACE}" will expand to "Hello, World" when the environment
 * variable PLACE="World". 
 */
int expand(char* orig, char* new, int newsize);

/* Redirect
 * line    The current line to be executed as a new process
 * returns a modified line with I/O redirection
 */
void redirect(char** line);
