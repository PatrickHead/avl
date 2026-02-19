#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "avl.h"

int compare(avl_node *a, avl_node *b)
{
  int va, vb;

  if (!a || !b) return 0;

  va = (int)((long long)a->value);
  vb = (int)((long long)b->value);

  if (va == vb) return 0;
  if (va < vb) return -1;
  if (va > vb) return 1;

  return 0;
}

int string_compare(avl_node *a, avl_node *b)
{
  char *va, *vb;
  int r;

  if (!a || !b) return 0;

  va = (char *)a->value;
  vb = (char *)b->value;

  r = strcmp(va, vb);

  if (!r) return 0;
  if (r < 0) return -1;
  if (r > 0) return 1;

  return 0;
}

void action(avl_node *n)
{
  if (!n) return;
  printf("%d ", (int)((long long)n->value));
}

void string_action(avl_node *n)
{
  if (!n) return;
  printf("%s\n", (char *)n->value);
}

char *keys[] = {
  "hello",
  "world",
  "this is fun",
  "another string",
  "123456",
  "my",
  "name",
  "is",
  "Kid",
  "Rock",
  NULL
};

int main() 
{ 
  avl *tree = NULL;
  avl_node *node = NULL; 
  avl_node *temp = NULL; 
  int rv = 0;
  char **s = NULL;

  printf("Simple integer tests\n");

  tree = avl_new();

  avl_set_cmp(tree, compare);

    /* Constructing tree given in the above figure */
  avl_insert(tree, avl_node_new(tree, (void *)10));
  avl_insert(tree, avl_node_new(tree, (void *)20));
  avl_insert(tree, avl_node_new(tree, (void *)30));
  avl_insert(tree, avl_node_new(tree, (void *)40));
  avl_insert(tree, avl_node_new(tree, (void *)50));
  avl_insert(tree, avl_node_new(tree, (void *)25));
  avl_insert(tree, avl_node_new(tree, (void *)49));
  avl_insert(tree, avl_node_new(tree, (void *)16));
  avl_insert(tree, avl_node_new(tree, (void *)26));
  avl_insert(tree, avl_node_new(tree, (void *)32));

  printf("Preorder traversal of the constructed AVL tree is\n"); 
  avl_walk(tree, avl_pre_order, (avl_action)action); 
  printf("\n");

  printf("Forward order traversal of the constructed AVL tree is\n");
  avl_walk(tree, avl_forward_order, (avl_action)action); 
  printf("\n");

  printf("Reverse order traversal of the constructed AVL tree is\n");
  avl_walk(tree, avl_reverse_order, (avl_action)action); 
  printf("\n");

  printf("Postorder traversal of the constructed AVL tree is\n");
  avl_walk(tree, avl_post_order, (avl_action)action); 
  printf("\n");

  printf("Tree order traversal of the constructed AVL tree is\n");
  avl_walk(tree, avl_tree_order, (avl_action)action);
  printf("\n");

  temp = avl_node_new(tree, (void *)16);
  node = avl_find(tree, temp);
  if (node) printf("16 found, node->key=%d\n", (int)((long long)node->value));
  else printf("16 not found\n");

  rv = avl_delete(tree, temp);
  printf("avl_delete() returned %d\n", rv);

  printf("Forward traversal after delete(16) is\n");
  avl_walk(tree, avl_forward_order, (avl_action)action); 
  printf("\n");

  node = avl_find(tree, temp);
  if (node) printf("16 found, node->key=%d\n", (int)((long long)node->value));
  else printf("16 not found\n");

  temp = avl_node_new(tree, (void *)25);
  node = avl_find(tree, temp);
  if (node) printf("25 found, node->key=%d\n", (int)((long long)node->value));
  else printf("25 not found\n");

  rv = avl_delete(tree, temp);
  printf("avl_delete() returned %d\n", rv);

  printf("Forward traversal after delete(25) is\n");
  avl_walk(tree, avl_forward_order, (avl_action)action); 
  printf("\n");

  node = avl_find(tree, temp);
  if (node) printf("25 found, node->key=%d\n", (int)((long long)node->value));
  else printf("25 not found\n");

  temp = avl_node_new(tree, (void *)40);
  node = avl_find(tree, temp);
  if (node) printf("40 found, node->key=%d\n", (int)((long long)node->value));
  else printf("40 not found\n");

  rv = avl_delete(tree, temp);
  printf("avl_delete() returned %d\n", rv);

  printf("Forward traversal after delete(40) is\n");
  avl_walk(tree, avl_forward_order, (avl_action)action); 
  printf("\n");

  node = avl_find(tree, temp);
  if (node) printf("40 found, node->key=%d\n", (int)((long long)node->value));
  else printf("40 not found\n");

  printf("Tree order traversal after deletions is\n");
  avl_walk(tree, avl_tree_order, (avl_action)action);
  printf("\n");

  avl_free(tree);

  printf("Simple string tests\n");

  tree = avl_new();

  avl_set_cmp(tree, string_compare);

  s = keys;
  while (*s)
  {
    rv = avl_insert(tree, avl_node_new(tree, strdup(*s)));
    printf("avl_insert(tree, \"%s\")=%d\n", *s, rv);
    ++s;
  }

  printf("Forward order traversal of the constructed AVL tree is\n");
  temp = avl_node_new(tree, "my");
  node = avl_find(tree, temp);
  printf("avl_find(tree, '%s')='%s'\n", (char *)temp->value, (char *)node->value);

  printf("Forward order traversal of the constructed AVL tree is\n");
  avl_walk(tree, avl_forward_order, (avl_action)string_action); 
  printf("\n");

  avl_delete(tree, temp);

  printf("Forward order traversal of the constructed AVL tree after removal of 'my'\n");
  avl_walk(tree, avl_forward_order, (avl_action)string_action); 
  printf("\n");

  avl_free(tree);

  return 0; 
} 

