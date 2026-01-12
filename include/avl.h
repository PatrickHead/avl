/*
 *  Copyright 2021,2024,2025 Patrick T. Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
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
 *  @file avl.h
 *  @brief Header file for simple C library for AVL trees
 */

#ifndef AVL_H
#define AVL_H

  /**
   *  @enum avl_order
   *  @brief AVL traversal order, used by avl_walk
   */

typedef enum
{
  avl_forward_order,  /**< In-order, left-root-right                              */
  avl_reverse_order,  /**< Reverse In-order, right-root-left                      */
  avl_pre_order,      /**< Pre-order, root-left-right                             */
  avl_post_order,     /**< Post-order, left-right-root                            */
  avl_tree_order      /**< Level-order, level 1 (root), then level 2, level 3 ... */
} avl_order;

  /**
   *  @typedef struct avl_node avl_node
   *  @brief   creates a type for @a avl_node struct
   */

typedef struct avl_node avl_node;

  /**
   *  @struct avl_node
   *  @brief AVL node structure
   */

struct avl_node
{
  avl_node *left;   /**<  points to left (lesser) node    */
  avl_node *right;  /**<  points to right (greater) node  */
  int height;       /**<  current height of node          */
  void *value;      /**<  generic key value for node      */
};

  /**
   *  @typedef avl_node *(*avl_new_node)(void);
   *  @brief   creates a type for function prototype to create a new @a avl_node
   *           struct
   */

typedef avl_node *(*avl_new_node)(void);

  /**
   *  @typedef avl_node *(*avl_dup_node)(avl_node *node);
   *  @brief   creates a type for function prototype to copy an @a avl_node
   *           struct
   */

typedef avl_node *(*avl_dup_node)(avl_node *node);

  /**
   *  @typedef void (*avl_free_node)(avl_node *node);
   *  @brief   creates a type for function prototype to free all memory
   *           allocated to an @a avl_node struct
   */

typedef void (*avl_free_node)(avl_node *node);

  /**
   *  @typedef int (*avl_cmp_node)(avl_node *a, avl_node *b);
   *  @brief   creates a type for function prototype to compare two @a avl_node
   *           structs
   */

typedef int (*avl_cmp_node)(avl_node *a, avl_node *b);

  /**
   *  @typedef struct avl avl
   *  @brief   creates a type for @a avl struct
   */

typedef struct avl avl;

  /**
   *  @struct avl
   *  @brief structure to track and maintain an AVL tree
   */

struct avl
{
  avl_node *root;           /**<  root of AVL tree                                           */
  int height;               /**<  current height of AVL tree                                 */
  avl_new_node new_node;    /**<  user supplied function to create a new @a avl_node         */
  avl_dup_node dup_node;    /**<  user supplied function to duplicate an @a avl_node         */
  avl_free_node free_node;  /**<  user supplied function to free an a @a avl_node            */
  avl_cmp_node cmp_node;    /**<  user supplied function to compare two @a avl_node structs  */
};

  /*
   *  AVL functions
   */

avl *avl_new(void);
avl *avl_dup(avl *tree);
void avl_free(avl *tree);
int avl_insert(avl *tree, avl_node *item);
int avl_delete(avl *tree, avl_node *target);
avl_node *avl_find(avl *tree, avl_node *target);
void avl_walk(avl *tree, avl_order order, void (*action)(avl_node *n));
void avl_set_new(avl *tree, avl_new_node new_node);
void avl_set_dup(avl *tree, avl_dup_node dup_node);
void avl_set_free(avl *tree, avl_free_node free_node);
void avl_set_cmp(avl *tree, avl_cmp_node cmp_node);

  /*
   *  AVL NODE functions
   */

avl_node *avl_node_new(avl *tree, void *value);
avl_node *avl_node_dup(avl *tree, avl_node *node);
void avl_node_free(avl *tree, avl_node *node);
int avl_node_cmp(avl *tree, avl_node *a, avl_node *b);

#endif //AVL_H
