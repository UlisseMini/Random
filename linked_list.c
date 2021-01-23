#include <stdio.h>
#include <stdlib.h>

/* Redefine malloc, calloc and free to count allocations and frees */
#define malloc(size) malloc(size); allocs++
#define calloc(nmemb, size) calloc(nmemb, size); allocs++
#define free(ptr) free(ptr); frees++

static int allocs = 0;
static int frees = 0;

/* Our linked list */

typedef struct Node {
  int data;
  struct Node* next;
} Node;

void display(Node* curr) {
  printf("(");
  for (; curr != NULL; curr = curr->next) {
    printf("%d", curr->data);

    // Ugly double check ):
    if (curr->next != NULL) printf(" ");
  }
  printf(")\n");
}

/* Append to the end of a linked list, complexity O(n) */
Node* append(Node* curr, int data) {
  while (curr->next != NULL) { curr = curr->next; }
  curr->next = calloc(1, sizeof(Node));
  curr->next->data = data;

  return curr->next;
}

/* Delete (and free) a node from a linked list. Note that this
 * function may crash or hang if the target is not in the list. */
void delete_node(Node* head, Node* target) {
  if (target == head) {
    free(head);
    return;
  }

  Node* prev = head;
  Node* curr = head->next;
  while (curr != target) {
    prev = curr;
    curr = curr->next;
  }
  /* curr is now target */
  prev->next = curr->next;
  free(curr);
}

/* Free every node in a linked list */
void free_list(Node* curr) {
  while (curr != NULL) {
    Node* next = curr->next;
    free(curr);
    curr = next;
  }
}

int main() {
  Node* n = calloc(1, sizeof(Node));
  n->data = 1;
  append(n, 2);
  append(n, 3);
  display(n);

  delete_node(n, n->next);
  display(n);

  delete_node(n, n->next);
  display(n);

  delete_node(n, n);

  /* free_list(n); */

  printf("done, %d allocations %d frees\n", allocs, frees);
  return 0;
}
