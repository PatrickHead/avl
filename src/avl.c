/*
 *  Copyright 2021,2024,2025 Patrick T. Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file avl.c
 * @brief Source code file for simple C library for AVL trees
 */

#include <stdlib.h>
#include <string.h>

#include "avl.h"

static avl_node *_avl_node_new(void *value);
static avl_node *_avl_insert(avl_node *node,
                             avl_node *item,
                             int (*compare)(avl_node *a, avl_node *b));
static avl_node *_avl_delete(avl_node *root,
                             avl_node *target,
                             int (*compare)(avl_node *a, avl_node *b),
                             void (*free_node)(avl_node *n));
static avl_node *_avl_find(avl_node *node,
                           avl_node *target,
                           int (*compare)(avl_node *a, avl_node *b));
static void _avl_destroy(avl_node *root,
                         int (*compare)(avl_node *a, avl_node *b),
                         void (*free_node)(avl_node *n));
static void _avl_walk(avl_node *root,
                      avl_order order,
                      void (*action)(avl_node *n));
static int height(avl_node *n);
static int max(int a, int b);
static avl_node *avl_rotate_right(avl_node *y);
static avl_node *avl_rotate_left(avl_node *x);
static int avl_get_balance(avl_node *n);
static avl_node *minimum_node(avl_node *n);
static void pre_order(avl_node *root, void (*action)(avl_node *n));
static void forward_order(avl_node *root, void (*action)(avl_node *n));
static void reverse_order(avl_node *root, void (*action)(avl_node *n));
static void post_order(avl_node *root, void (*action)(avl_node *n));
static void tree_order(avl_node *root, void (*action)(avl_node *n));
static void __tree_order(avl_node *root,
                         void (*action)(avl_node *n), int height);
static void dup_tree(avl *tree, avl_node **new_root, avl_node *old_root);

    /*
     * public functions
     */

      /*
       *  AVL functions
       */

  /**
   *  @fn avl *avl_new(void)
   *
   *  @brief Create an avl tree
   *
   *  @par Parameters
   *       None.
   *
   *  @return pointer to new @a avl struct
   */

avl *avl_new(void)
{
  avl *tree = NULL;

  tree = malloc(sizeof(avl));
  if (tree) memset(tree, 0, sizeof(avl));

  return tree;
}

  /**
   *  @fn avl *avl_dup(avl *tree)
   *
   *  @brief Creates deep copy of *p tree
   *
   *  @param tree - pointer to existing AVL tree
   *
   *  @return pointer to new @a avl struct
   */

avl *avl_dup(avl *tree)
{
  avl *new_tree = NULL;

  if (!tree) goto exit;

  new_tree = malloc(sizeof(avl));
  if (!new_tree) goto exit;

  memcpy(new_tree, tree, sizeof(avl));
  new_tree->root = NULL;
  new_tree->height = 0;

  dup_tree(tree, &new_tree->root, tree->root);

exit:
  return new_tree;
}

  /**
   *  @fn void avl_free(avl *tree)
   *
   *  @brief frees all memory allocated to @p tree
   *
   *  @param tree - pointer to existing AVL tree
   *
   *  @par Returns
   *       Nothing.
   */

void avl_free(avl *tree)
{
  if (!tree) return;

  if (tree->root) _avl_destroy(tree->root, tree->cmp_node, tree->free_node);
  free(tree);
}

  /**
   *  @fn int avl_insert(avl *tree, avl_node *item)
   *
   *  @brief inserts @p item into @p tree
   *
   *  @param tree - pointer to existing AVL tree
   *  @param item - pointer to AVL NODE to insert
   *
   *  @return 0 on success, -1 on failure
   */

int avl_insert(avl *tree, avl_node *item)
{
  int rv = -1;
  avl_node *new_root;

  if (!tree || !item) goto exit;

  new_root = _avl_insert(tree->root, item, tree->cmp_node);
  if (new_root)
  {
    tree->root = new_root;
    tree->height = height(tree->root);
    rv = 0;
  }

exit:
  return rv;
}

  /**
   *  @fn int avl_delete(avl *tree, avl_node *target)
   *
   *  @brief deletes @p item from @p tree
   *
   *  @param tree - pointer to existing AVL tree
   *  @param target - pointer to AVL NODE to delete
   *
   *  @return 0 on success, -1 on failure
   */

int avl_delete(avl *tree, avl_node *target)
{
  int rv = -1;
  avl_node *new_root;

  if (!tree || !target) goto exit;

  new_root = _avl_delete(tree->root, target, tree->cmp_node, tree->free_node);
  if (new_root)
  {
    tree->root = new_root;
    tree->height = height(tree->root);
    rv = 0;
  }

exit:
  return rv;
}

  /**
   *  @fn avl_node *avl_find(avl *tree, avl_node *target)
   *
   *  @brief searches @p tree for @p target
   *
   *  @param tree - pointer to existing AVL tree
   *  @param target - pointer to AVL NODE to search for
   *
   *  @return pointer to @a avl_node on success, NULL on failure
   */

avl_node *avl_find(avl *tree, avl_node *target)
{
  avl_node *node = NULL;

  if (!tree || !target) goto exit;

  node = _avl_find(tree->root, target, tree->cmp_node);

exit:
  return node;
}

  /**
   *  @fn void avl_walk(avl *tree, avl_order order, void (*action)(avl_node *n))
   *
   *  @brief walk an AVL tree in requested order calling @p action on each node
   *
   *  @param tree - pointer to @a avl struct
   *  @param order - @a avl_order; traversal order of nodes in tree
   *  @param action - function that gets called for each encountered node
   *
   *  @par Returns
   *    Nothing.
   */

void avl_walk(avl *tree, avl_order order, void (*action)(avl_node *n))
{
  if (!tree || !action) goto exit;

  _avl_walk(tree->root, order, action);

exit:
  return;
}

  /**
   *  @fn void avl_set_new(avl *tree, avl_node *(*new_node)(void))
   *
   *  @brief sets function used by @p tree for created a new @a avl_node
   *
   *  @param tree - pointer to @a avl struct
   *  @param new_node - function that creates a new @a avl_node for this tree
   *
   *  @par Returns
   *    Nothing.
   */

void avl_set_new(avl *tree, avl_node *(*new_node)(void))
{
  if (tree) tree->new_node = new_node;
}

  /**
   *  @fn void avl_set_dup(avl *tree, avl_node *(*dup_node)(avl_node *node))
   *
   *  @brief sets function used by @p tree for duplicating an @a avl_node
   *
   *  @param tree - pointer to @a avl struct
   *  @param dup_node - function that duplicates an @a avl_node for this tree
   *
   *  @par Returns
   *    Nothing.
   */

void avl_set_dup(avl *tree, avl_node *(*dup_node)(avl_node *node))
{
  if (tree) tree->dup_node = dup_node;
}

  /**
   *  @fn void avl_set_free(avl *tree, void (*free_node)(avl_node *node))
   *
   *  @brief sets function used by @p tree for freeing an @a avl_node
   *
   *  @param tree - pointer to @a avl struct
   *  @param free_node - function that frees an @a avl_node for this tree
   *
   *  @par Returns
   *    Nothing.
   */

void avl_set_free(avl *tree, void (*free_node)(avl_node *node))
{
  if (tree) tree->free_node = free_node;
}

  /**
   *  @fn void avl_set_cmp(avl *tree, int (*cmp_node)(avl_node *a, avl_node *b))
   *
   *  @brief sets function used by @p tree for comparing two @a avl_node
   *
   *  @param tree - pointer to @a avl struct
   *  @param cmp_node - function that compares two @a avl_node for this tree
   *
   *  @par Returns
   *    Nothing.
   */

void avl_set_cmp(avl *tree, int (*cmp_node)(avl_node *a, avl_node *b))
{
  if (tree) tree->cmp_node = cmp_node;
}

      /*
       *  AVL NODE functions
       */

  /**
   *  @fn avl_node *avl_node_new(avl *tree, void *value)
   *
   *  @brief creates a new @a avl_node
   *
   *  @param tree - pointer to existing AVL tree
   *  @param value - pointer to payload that is attached to the node
   *
   *  @return pointer to @a avl_node on success, NULL on failure
   */

avl_node *avl_node_new(avl *tree, void *value)
{
  avl_node *node = NULL;

  if (!tree) goto exit;

  if (tree->new_node) node = tree->new_node();
  else node = _avl_node_new(value);

exit:
  return node;
}

  /**
   *  @fn avl_node *avl_node_dup(avl *tree, avl_node *node)
   *
   *  @brief duplicates an @a avl_node
   *
   *  @param tree - pointer to existing AVL tree
   *  @param node - pointer to existing AVL NODE struct
   *
   *  @return pointer to @a avl_node on success, NULL on failure
   */

avl_node *avl_node_dup(avl *tree, avl_node *node)
{
  avl_node *new_node = NULL;

  if (!tree || !node) goto exit;

  if (tree->dup_node) new_node = tree->dup_node(node);
  else
  {
    new_node = _avl_node_new(NULL);
    if (!new_node) goto exit;
    new_node->value = node->value;
  }

exit:
  return new_node;
}

  /**
   *  @fn void avl_node_free(avl *tree, avl_node *node)
   *
   *  @brief frees memory allocated to @p node
   *
   *  @param tree - pointer to existing AVL tree
   *  @param node - pointer to existing AVL NODE struct
   *
   *  @par Returns
   *       Nothing.
   */

void avl_node_free(avl *tree, avl_node *node)
{
  if (!tree || !node) return;

  if (tree->free_node) tree->free_node(node);
  else free(node);
}

  /**
   *  @fn int avl_node_cmp(avl *tree, avl_node *a, avl_node *b)
   *
   *  @brief compares @p a and @p b
   *
   *  @param tree - pointer to existing AVL tree
   *  @param a - pointer to existing AVL NODE struct
   *  @param b - pointer to existing AVL NODE struct
   *
   *  @return -1 if a<b, 0 if a==b, 1 if a>b
   */

int avl_node_cmp(avl *tree, avl_node *a, avl_node *b)
{
  if (!tree || !a || !b) return 0;
  if (!tree->cmp_node) return 0;

  return tree->cmp_node(a, b);
}

    /*
     * private functions
     */

  /**
   *  @fn avl_node *_avl_insert(avl_node *node,
   *                            avl_node *item,
   *                            int (*compare)(avl_node *a, avl_node *b))
   *
   *  @brief Insert a node into the subtree rooted with node
   *
   *  @param node - pointer to existing @a avl_node, root of insertion point
   *  @param item - pointer to existing @a avl_node, new node to insert
   *  @param compare - comparison function used to determine order of two nodes
   *
   *  @return pointer to new root of subtree
   */

static avl_node *_avl_insert(avl_node *node,
                             avl_node *item,
                             int (*compare)(avl_node *a, avl_node *b))
{
  int pos = 0;

    /*
     * Perform the normal BST insertion
     */

  if (!node) return(item);
  if (!item) return node;

  pos = compare(item, node);

  if (pos < 0) node->left = _avl_insert(node->left, item, compare);
  else if (pos > 0) node->right = _avl_insert(node->right, item, compare);
  else return node;

    /*
     * Update height of this ancestor node
     */

  node->height = 1 + max(height(node->left), height(node->right));

    /*
     * Get the balance factor of this ancestor node
     */

  int balance = avl_get_balance(node);

    /*
     * If this node becomes unbalanced, then there are 4 cases
     */

      /*
       * Left Left Case
       */

  if ((balance > 1) && (compare(item, node->left) < 0))
    return avl_rotate_right(node);

      /*
       * Right Right Case
       */

  if ((balance < -1) && (compare(item, node->right) > 0))
    return avl_rotate_left(node);

      /*
       * Left Right Case
       */

  if ((balance > 1) && (compare(item, node->left) > 0))
  {
    node->left = avl_rotate_left(node->left);
    return avl_rotate_right(node);
  }

      /*
       * Right Left Case
       */

  if ((balance < -1) && (compare(item, node->right) < 0))
  {
    node->right = avl_rotate_right(node->right);
    return avl_rotate_left(node);
  }

    /*
     * return the (unchanged) node pointer
     */

  return node;
}

  /**
   *  @fn avl_node *_avl_delete(avl_node *root,
   *                            avl_node *target,
   *                            int (*compare)(avl_node *a, avl_node *b),
   *                            void (*free_node)(avl_node *n))
   *
   *  @brief deletes a node with given key given root to delete from
   *
   *  @param root - pointer to @a avl_node which is root of tree to delete from
   *  @param target - pointer to @a avl_node containing target key to delete
   *  @param compare - comparison function used to determine order of two nodes
   *  @param free_node - function used to free memory allocated to deleted node
   *
   *  @return pointer to new root of subtree
   */

static avl_node *_avl_delete(avl_node *root,
                             avl_node *target,
                             int (*compare)(avl_node *a, avl_node *b),
                             void (*free_node)(avl_node *n))
{
  int pos = 0;
  void *root_val;
  avl_node *child, temp;

    /*
     * Standard BST delete
     */

  if (!root || !target || !compare) return root;

  pos = compare(target, root);

  if (pos < 0)
    root->left = _avl_delete(root->left, target, compare, free_node);
  else if(pos > 0)
    root->right = _avl_delete(root->right, target, compare, free_node);
  else
  {
      /*
       * node with zero or one child
       */

    if((root->left == NULL) || (root->right == NULL))
    {
      child = root->left ? root->left : root->right;
      if (child)
      {
        temp = *root;
        *root = *child;
        *child = temp;
      }
      if (!child)
      {
        child = root;
        root = NULL;
      }
      if (free_node) free_node(child);
      else free(child);
    }

      /*
       * node with two children
       */

    else
    {
        /*
         * get the inorder successor (smallest in the right subtree)
         */

      avl_node* temp = minimum_node(root->right);

        /*
         * copy the inorder successor's data to this node
         */

      root_val = root->value;
      root->value = temp->value;
      temp->value = root_val;

        /*
         * delete the inorder successor
         */

      root->right = _avl_delete(root->right, temp, compare, free_node);
    }
  }

    /*
     * If the tree had only one node then return
     */

  if (root == NULL) return root;

    /*
     * Update the height of the current tree
     */

  root->height = 1 + max(height(root->left), height(root->right));

    /*
     * Get balance of this node
     */

  int balance = avl_get_balance(root);

      /*
       * If this node becomes unbalanced, then there are 4 cases
       */

    /*
     * left-left
     */

  if (balance > 1 && avl_get_balance(root->left) >= 0)
    return avl_rotate_right(root);

    /*
     * left-right
     */

  if (balance > 1 && avl_get_balance(root->left) < 0)
  {
    root->left = avl_rotate_left(root->left);
    return avl_rotate_right(root);
  }

    /*
     * right-right
     */

  if (balance < -1 && avl_get_balance(root->right) <= 0)
    return avl_rotate_left(root);

    /*
     * right-left
     */

  if (balance < -1 && avl_get_balance(root->right) > 0)
  {
    root->right = avl_rotate_right(root->right);
    return avl_rotate_left(root);
  }

  return root;
}

  /**
   *  @fn avl_node *_avl_find(avl_node *root,
   *                          avl_node *target,
   *                          int (*compare)(avl_node *a, avl_node *b))
   *
   *  @brief find node in @p root that matches key in @p target
   *
   *  @param root - pointer to @a avl_node which is root of tree to search
   *  @param target - pointer to @a avl_node containing key to match
   *  @param compare - comparison function used to determine order of two nodes
   *
   *  @return pointer to @a avl_node that matches @p target, or NULL if no match is found
   */

static avl_node *_avl_find(avl_node *root,
                           avl_node *target,
                           int (*compare)(avl_node *a, avl_node *b))
{
  int pos = 0;

  if (!root || !target) return NULL;

  pos = compare(target, root);

  if (pos == 0) return root;
  if (pos < 0) return _avl_find(root->left, target, compare);
  if (pos > 0) return _avl_find(root->right, target, compare);

  return NULL;
}

  /**
   *  @fn void _avl_destroy(avl_node *root,
   *                        int (*compare)(avl_node *a, avl_node *b),
   *                        void (*free_node)(avl_node *n))
   *
   *  @brief destroy an entire AVL tree
   *
   *  @param root - pointer to @a avl_node that is root of tree
   *  @param compare - comparison function used to determine order of two nodes
   *  @param free_node - function used to free memory allocated to deleted node
   *
   *  @par Returns
   *    Nothing.
   */

static void _avl_destroy(avl_node *root,
                         int (*compare)(avl_node *a, avl_node *b),
                         void (*free_node)(avl_node *n))
{
  if (!root) return;

  while (root) root = _avl_delete(root, root, compare, free_node);
}

  /**
   *  @fn void _avl_walk(avl_node *root,
   *                     avl_order order,
   *                     void (*action)(avl_node *n))
   *
   *  @brief walk an AVL tree, in requested order
   *
   *  @param root - pointer to @a avl_node that is root of tree
   *  @param order - @a avl_order traversal order of nodes in tree
   *  @param action - function that gets called for each traversed node
   *
   *  @par Returns
   *    Nothing.
   */

static void _avl_walk(avl_node *root,
                      avl_order order,
                      void (*action)(avl_node *n))
{
  if (!root) return;

  switch (order)
  {
    case avl_forward_order: forward_order(root, action); break;
    case avl_reverse_order: reverse_order(root, action); break;
    case avl_pre_order: pre_order(root, action); break;
    case avl_post_order: post_order(root, action); break;
    case avl_tree_order: tree_order(root, action); break;
  }
}

  /*
   *
   * Determines height of tree below node
   *
   */

  /**
   *  @fn int height(avl_node *n)
   *
   *  @brief calculate height of tree at node @p n
   *
   *  @param n - pointer to node in tree
   *
   *  @return height of tree at node @p n
   */

static int height(avl_node *n)
{
  if (!n) return 0;
  return n->height;
}

  /**
   *  @fn int max(int a, int b)
   *
   *  @brief return maximum of two integers
   *
   *  @param a - first integer
   *  @param b - second integer
   *
   *  @return maximum of a or b
   */

static int max(int a, int b) { return (a > b) ? a : b; }

  /**
   *  @fn avl_node *avl_rotate_right(avl_node *y)
   *
   *  @brief right rotate subtree rooted at @p y
   *
   *  @param y - pointer to @a avl_node in tree
   *
   *  @return pointer to @a avl_node that is new root at @p y
   */

static avl_node *avl_rotate_right(avl_node *y)
{
  avl_node *x = y->left;
  avl_node *T2 = x->right;

    // Perform rotation
  x->right = y;
  y->left = T2;

    // Update heights
  y->height = max(height(y->left), height(y->right))+1;
  x->height = max(height(x->left), height(x->right))+1;

    // Return new root
  return x;
}

  /**
   *  @fn avl_node *avl_rotate_left(avl_node *x)
   *
   *  @brief left rotate subtree rooted at @p x
   *
   *  @param x - pointer to @a avl_node in tree
   *
   *  @return pointer to @a avl_node that is new root at @p x
   */

static avl_node *avl_rotate_left(avl_node *x)
{
  avl_node *y = x->right;
  avl_node *T2 = y->left;

    // Perform rotation
  y->left = x;
  x->right = T2;

    // Update heights
  x->height = max(height(x->left), height(x->right))+1;
  y->height = max(height(y->left), height(y->right))+1;

    // Return new root
  return y;
}

  /**
   *  @fn int avl_get_balance(avl_node *n)
   *
   *  @brief returns balance factor of @p n
   *
   *  The balance factor is the difference of the sub-tree height on the left side of @p n
   *  and the right side of @p n.
   *
   *  @param n - pointer to @a avl_node in tree
   *
   *  @return balance factor
   */

static int avl_get_balance(avl_node *n)
{
  if (!n) return 0;
  return height(n->left) - height(n->right);
}

  /**
   *  @fn avl_node *minimum_node(avl_node *n)
   *
   *  @brief returns @a avl_node under @p n with minimum value
   *
   *  Given a non-empty binary search tree, return the node with minimum key
   *  value found in that tree.  Note that the entire tree does not need to be
   *  searched.
   *
   *  @param n - pointer to @a avl_node in tree
   *
   *  @return pointer to @a avl_node under @p n with minumum value
   */

static avl_node *minimum_node(avl_node *n)
{
  avl_node *current = n;

    /* loop down to find the leftmost leaf */
  while (current->left != NULL) current = current->left;

  return current;
}

  /**
   *  @fn void pre_order(avl_node *root, void (*action)(avl_node *n))
   *
   *  @brief Walk the sub-tree under @p n in pre-order: root-left-right
   *
   *  @param root - pointer to @a avl_node in tree
   *  @param action - pointer to function that gets called at each node
   *
   *  @par Returns
   *  Nothing.
   */

static void pre_order(avl_node *root, void (*action)(avl_node *n))
{
  if (!root || !action) return;

  action(root);
  pre_order(root->left, action);
  pre_order(root->right, action);
}

  /**
   *  @fn void forward_order(avl_node *root, void (*action)(avl_node *n))
   *
   *  @brief Walk the sub-tree under @p n in forward-order: left-root-right
   *
   *  @param root - pointer to @a avl_node in tree
   *  @param action - pointer to function that gets called at each node
   *
   *  @par Returns
   *  Nothing.
   */

static void forward_order(avl_node *root, void (*action)(avl_node *n))
{
  if (!root || !action) return;

  if (root->left) forward_order(root->left, action);
  action(root);
  if (root->right) forward_order(root->right, action);
}

  /**
   *  @fn void reverse_order(avl_node *root, void (*action)(avl_node *n))
   *
   *  @brief Walk the sub-tree under @p n in reverse-order: right-root-left
   *
   *  @param root - pointer to @a avl_node in tree
   *  @param action - pointer to function that gets called at each node
   *
   *  @par Returns
   *  Nothing.
   */

static void reverse_order(avl_node *root, void (*action)(avl_node *n))
{
  if (!root || !action) return;

  if (root->right) reverse_order(root->right, action);
  action(root);
  if (root->left) reverse_order(root->left, action);
}

  /**
   *  @fn void post_order(avl_node *root, void (*action)(avl_node *n))
   *
   *  @brief Walk the sub-tree under @p n in post-order: left-right-root
   *
   *  @param root - pointer to @a avl_node in tree
   *  @param action - pointer to function that gets called at each node
   *
   *  @par Returns
   *  Nothing.
   */

static void post_order(avl_node *root, void (*action)(avl_node *n))
{
  if (!root || !action) return;

  post_order(root->left, action);
  post_order(root->right, action);
  action(root);
}

  /**
   *  @fn void tree_order(avl_node *root, void (*action)(avl_node *n))
   *
   *  @brief Walk the sub-tree under @p n in tree-order, AKA level-order:
   *    level1-level2-...-levelN
   *
   *  @param root - pointer to @a avl_node in tree
   *  @param action - pointer to function that gets called at each node
   *
   *  @par Returns
   *  Nothing.
   */

static void tree_order(avl_node *root, void (*action)(avl_node *n))
{
  int max;
  int i;

  if (!root || !action) return;

  max = root->height;

  for (i = max; i >= 1; --i)
    __tree_order(root, action, i);
}

  /**
   *  @fn void __tree_order(avl_node *root,
   *                        void (*action)(avl_node *n),
   *                        int height)
   *
   *  @brief recursive helper function for tree_order()
   *
   *  @param root - pointer to @a avl_node in tree
   *  @param action - pointer to function that gets called at each node
   *  @param height - height of tree under @p n
   *
   *  @par Returns
   *  Nothing.
   */

static void __tree_order(avl_node *root, void (*action)(avl_node *n), int height)
{
  if (!root || !action) return;

  
  if (root->height == height) action(root);

  __tree_order(root->left, action, height);
  __tree_order(root->right, action, height);
}

  /**
   *  @fn avl_node *_avl_node_new(void *value)
   *
   *  @brief Create a node
   *
   *  @param value - generic pointer to any object or coerced instrisic value
   *
   *  @return pointer to new @a avl_node
   */

static avl_node *_avl_node_new(void *value)
{
  avl_node *n = NULL;

  if ((n = (avl_node *)malloc(sizeof(avl_node))))
    memset(n, 0, sizeof(avl_node));

  n->value = value;
  n->height = 1;

  return n;
}

  /**
   *  @fn void dup_tree(avl *tree, avl_node **new_root, avl_node *old_root)
   *
   *  @brief make a deep copy of an entire AVL tree
   *
   *  @param tree - pointer to existing @a avl struct
   *  @param new_root - address of pointer to root @a avl_node of new tree
   *  @param old_root - pointer to root @a avl_node of existing tree
   *
   *  @par Returns
   *       Nothing.
   */

static void dup_tree(avl *tree, avl_node **new_root, avl_node *old_root)
{
  avl_node *node = NULL;

  if (!tree || !new_root || !old_root) return;

  if (old_root->left) dup_tree(tree, new_root, old_root->left);

  node = avl_node_dup(tree, old_root);
  if (node) *new_root = _avl_insert(*new_root, node, tree->cmp_node);

  if (old_root->right) dup_tree(tree, new_root, old_root->right);
}

