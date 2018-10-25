#include "target.h"

/* List Append
 *
 */
void list_append(node* head, char* targ);




void main(){
  node* head = NULL;
  
}

node* list_append(node* head, char* targ){
  node* temp = (node *)malloc(sizeof(node));
  temp->next = NULL;
  temp->target = targ;

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
