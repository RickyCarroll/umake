#include "target.h"
#include "umake.h"


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

/* List Append Rule
 * head - head of rule list
 * temp - rule to be added
 */
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
 * argcp - pointer to the current line's argument count
 * argsp - pointer to the current line's arguments
 * head - head of the rule list
 */
node* list_create_node(int argcp, char** args, rule* head){
  node* temp = (node*)malloc(sizeof(node));
  temp->next = NULL;
  if (args[0][strlen(args[0])-1] == ':'){
    args[0][strlen(args[0])-1] = '\0';
  }
  temp->target = strdup(args[0]);
  temp->dependencies = malloc(sizeof(char*)*(argcp-1));
  for (int i = 1; i < argcp-1; i++){
    temp->dependencies[i-1] = strdup(args[i]);
  }
  temp->rules = (rule*)malloc(sizeof(rule));
  temp->rules = head;
  return temp;
}

/* List Create Rule
 * line - pointer to the current line
 * linelen - length of the current line
 */
rule* list_create_rule(char* line, int linelen){
  rule* temp =(rule*)malloc(sizeof(rule));
  temp->next = NULL;
  temp->rule = strdup(line);
  return temp;
}

/* List Size
 * head - head of either node list or rule list
 */
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


/* List Target Depend
 * argcp - number of arguments of current line
 * args - argunets for the current line
 * rhead - head of the rule list
 * nhead - head of the node list
 */
node* list_target_depend(int argcp, char** args, rule* rhead, node* nhead){
  if (rhead != NULL){
    node* node = list_create_node(argcp, args, rhead);
    nhead = list_append_node(nhead, node);
  }
  return nhead;
}

/* List Rule
 * line - current line un-parsed
 * linelen - length of line
 * head - head of the rule list
 */
rule* list_rule(char* line, int linelen, rule* head){
      rule* new = list_create_rule(line, linelen);
      head = list_append_rule(head, new);
      return head;
}

/* List Search
 * head - head of the node list
 * targname - name of the target
 */
node* list_search(node* head, const char* targname){
  if (head == NULL){
    return NULL;
  }
  else{
    node* iter = head;
    while(iter->next != NULL && strcmp(iter->target, targname) != 0){
      iter = iter->next;
    }
    if (strcmp(iter->target, targname) == 0){
      return iter;
    }
  }
  return NULL;
}

/* List Free Node
 * node - head of node list
 */
void list_free_node(node* node){
  if(node->next != NULL){
    list_free_node(node->next);
  }else{
    return;
  }
  list_free_rule(node->rules);
  free(node->rules);
  free(node->target);
  for(int i = 0; node->dependencies[i] != NULL; i++){
    free(node->dependencies[i]);
  }
  free(node->dependencies);
  free(node->next);
}

/* List Free Rule
 * rule - rule struct
 */
void list_free_rule(rule* rule){
  if(rule->next != NULL){
    list_free_rule(rule->next);
  }else{
    return;
  }
  free(rule->rule);
  free(rule->next);
}

void process_rules(rule* rule){
  while(rule->next != NULL){
    char* curline = strdup(&rule->rule[1]);
    processline(curline);
    rule = rule->next;
  }
  processline(&rule->rule[1]);
}
