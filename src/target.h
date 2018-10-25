typedef struct targetst {
  struct targetst* next;
  char* target;
  char** dependencies;
  char** rules;
} node;

#define EMPTY NULL;
