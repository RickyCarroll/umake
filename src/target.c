#include "target.h"


/* List Append
 * head   - the first node in the linked list
 * temp   - the node to be added to the end of the linked list
 */
node* list_append_node(node* head, node* temp){
  if (head == NULL){
    head = temp;
  }
  else{
    node* iter = head;
    while(iter->next != NULL){
      iter = iter->next;
    }
    iter->next = temp;
  }
  return head;
}

rule* list_append_rule(rule* head, rule* temp){
  if (head == NULL){
    head = temp;
  }
  else{
    rule* iter = head;
    while(iter->next != NULL){
      iter = iter->next;
    }
    iter->next = temp;
  }
  return head;
}

/* List Create Target Node
 *
 *
 */
node* list_create_node(int argcp, char** args, rule* head){
  node* temp = (node*)malloc(sizeof(node));
  temp->next = NULL;
  temp->target = malloc(sizeof(char)*(strlen(args[0])+1));
  if (args[0][strlen(args[0])-1] == ':'){
    args[0][strlen(args[0])-1] = '\0';
  }
  temp->target = strdup(args[0]);
  temp->dependencies = malloc(sizeof(char*)*(argcp-1));
  for (int i = 1; i < argcp-1; i++){
    temp->dependencies[i] = malloc(sizeof(char)*(strlen(args[i])+1));
    temp->dependencies[i] = strdup(args[i]);
  }
  temp->rules = (rule*)malloc(sizeof(rule));
  temp->rules = head;
  return temp;
}

rule* list_create_rule(char* line, int linelen){
  rule* temp =(rule*)malloc(sizeof(rule));
  temp->next = NULL;
  temp->rule = malloc(sizeof(char)*linelen);
  strcpy(temp->rule, line);
  return temp;
}

int list_size(void* head){
  int size = 1;
  if (head == NULL){
    return 0;
  }
  else{
    node* iter = head;
    while(iter->next != NULL){
      iter = iter->next;
      size++;
    }
  }
  return size;
}

node* list_target_depend(int argcp, char** args, rule* rhead, node* nhead){
  if (rhead != NULL){
    node* node = list_create_node(argcp, args, rhead);
    nhead = list_append_node(nhead, node);
  }
  return nhead;
}

rule* list_rule(char* line, int linelen, rule* head){
      rule* new = list_create_rule(line, linelen);
      head = list_append_rule(head, new);
      return head;
}

rule* list_search(node* head, const char* targname){
  if (head == NULL){
    return NULL;
  }
  else{
    node* iter = head;
    while(iter->next != NULL && strcmp(iter->target, targname) != 0){
      iter = iter->next;
    }
    if (strcmp(iter->target, targname) == 0){
      return iter->rules;
    }
  }
  fprintf(stderr, "target: %s does not exist\n", targname);
  exit(0);
}
