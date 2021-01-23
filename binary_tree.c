#include <stdio.h>
#include <stdlib.h>

/* Redefine malloc, calloc and free to count allocations and frees */
#define malloc(size) malloc(size); allocs++
#define calloc(nmemb, size) calloc(nmemb, size); allocs++
#define free(ptr) free(ptr); frees++

static int allocs = 0;
static int frees = 0;

/* Our binary tree */

typedef struct TNode {
  int data;
  struct TNode* left;
  struct TNode* right;
} TNode;

TNode* TNode_new(int data) {
  TNode* node = calloc(1, sizeof(TNode));
  node->data = data;
  return node;
}

/* TODO: Return number deleted
 * TODO: Convert to iterative loop */
void TNode_free(TNode* tree) {
  if (tree->left != NULL) TNode_free(tree->left);
  if (tree->right != NULL) TNode_free(tree->right);

  free(tree);
}

/* Add data to a tree, return a pointer to the added node. */
TNode* TNode_add(TNode* tree, int data) {
  if (data > tree->data) {
    if (tree->right == NULL) {
      tree->right = TNode_new(data);
      return tree->right;
    } else {
      return TNode_add(tree->right, data);
    }
  } else {
    if (tree->left == NULL) {
      tree->left = TNode_new(data);
      return tree->left;
    } else {
      return TNode_add(tree->left, data);
    }
  }
}

void TNode_display_ident(TNode* tree, int ident) {
  if (tree == NULL) return;

  /* TODO: Do this the non retarded way */
  for (int i = 0; i < ident; i++) { printf(" "); }

  printf("%d\n", tree->data);
  TNode_display_ident(tree->left, ident - 2);
  TNode_display_ident(tree->right, ident + 2);
}

void TNode_display(TNode* tree) {
  TNode_display_ident(tree, 10);
}

/* Display the tree in order */
void TNode_display_inorder(TNode* tree) {
  if (tree->left  != NULL) TNode_display_inorder(tree->left);
  printf("%d ", tree->data);
  if (tree->right != NULL) TNode_display_inorder(tree->right);
}

/* Test it out! */

int main() {
  TNode* tree = TNode_new(3);
  tree->right = TNode_new(5);
  tree->right->left = TNode_new(4);
  tree->left = TNode_new(2);
  tree->left->left = TNode_new(1);

  TNode_add(tree, 6);
  TNode_add(tree, 0);

  TNode_display(tree);
  TNode_display_inorder(tree);

  TNode_free(tree);

  printf("\n\ndone, %d allocations %d frees\n", allocs, frees);
  return 0;
}
