#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef TARGET_H
#define TARGET_H

typedef struct rulest{
  struct rulest* next;
  char* rule;
} rule;

typedef struct targetst {
  struct targetst* next;
  char*  target;
  char** dependencies;
  rule*  rules;
} node;

/* List Append Rule
 * appends the temp rule to the end of the list or if it's empty makes it the head of the list
 * returns a pointer to the head of the rule list
 */
rule* list_append_rule(rule* head, rule* temp);
/*List Append Node
 * appends the temp node to the end of the list or if it's empty makes it the head of the list
 * returns pointer to the head of the node list
 */
node* list_append_node(node* head, node* temp);
/* List Create Node
 * creates and populates target node using rules that have been been put in rules and using
 * args for the target and dependencies
 * returns the pointer to the node struct
 */
node* list_create_node(int argc, char** args, rule* rules);
/*List Create Rule
 * creates and pupulates rule node using line
 * returns a pointer to the rule struct
 */
rule* list_create_rule(char* line, int linelen);
/* List Size
 * Traverse list and find size
 * return size
 */
int   list_size(void* head);
/* List Target Depend
 * uses List Create Depend and List Append Node in one function that main calls
 * returns head of the node list
 */
node* list_target_depend(int argcp, char** args, rule* rhead, node* nhead);
/* List Rule
 * uses List Create Rule and List Append Rule in one function that main calls
 * returns head of the rule list
 */
rule* list_rule(char* line, int linelen, rule* head);
/* List Search
 * searches list of nodes for target name
 * returns pointer to node structt
 */
node* list_search(node* head, const char* targname);
/* List Free Node
 * frees all nodes structs in list recursively
 */
void list_free_node(node* node);
/* List Free Rule
 * frees all rule structs in list recursively
 */
void list_free_rule(rule* rule);


#define EMPTY NULL;

#endif
