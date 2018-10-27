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


rule* list_append_rule(rule* head, rule* temp);

node* list_append_node(node* head, node* temp);

node* list_create_node(int argc, char** args, rule* rules);

rule* list_create_rule(char* line, int linelen);

int   list_size(void* head);

node* list_target_depend(int argcp, char** args, rule* rhead, node* nhead);

rule* list_rule(char* line, int linelen, rule* head);

rule* list_search(node* head, const char* targname);


#define EMPTY NULL;

#endif
