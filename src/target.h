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
 *
 */
rule* list_append_rule(rule* head, rule* temp);
/*List Append Node
 *
 */
node* list_append_node(node* head, node* temp);
/* List Create Node
 * creates and populates target node
 */
node* list_create_node(int argc, char** args, rule* rules);
/*List Create Rule
 * creates and pupulates rule node
 */
rule* list_create_rule(char* line, int linelen);
/* List Size
 * 
 */
int   list_size(void* head);
/* List Target Depend
 * uses List Create Depend and List Append Node
 */
node* list_target_depend(int argcp, char** args, rule* rhead, node* nhead);
/* List Rule
 * uses List Create Rule and List Append Rule
 */
rule* list_rule(char* line, int linelen, rule* head);
/* List Search
 * searches list of nodes for targname
 */
node* list_search(node* head, const char* targname);

void list_free_node(node* node);

void list_free_rule(rule* rule);


#define EMPTY NULL;

#endif
