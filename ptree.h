/* Copyright (c) 2014 Anton Titov.
 * Copyright (c) 2014 pCloud Ltd.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of pCloud Ltd nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL pCloud Ltd BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PSYNC_TREE_H
#define _PSYNC_TREE_H

#include "pcompiler.h"
#include <stdlib.h>
#include <stddef.h>

typedef struct _psync_tree {
  struct _psync_tree *left;
  struct _psync_tree *right;
  struct _psync_tree *parent;
  long int height;
} psync_tree;

#define PSYNC_TREE_EMPTY NULL

#define psync_tree_isempty(l) ((l)==NULL)
#define psync_tree_element(a, t, n) ((t *)((char *)(a)-offsetof(t, n)))
#define psync_tree_for_each(a, l) for (a=psync_tree_get_first(l); a!=NULL; a=psync_tree_get_next(a))
#define psync_tree_for_each_safe(a, b, l) for (a=(l)->next, b=a->next; psync_prefetch(b), a!=(l); a=b, b=b->next)

#define psync_tree_for_each_element(a, l, t, n) for (a=psync_tree_element(psync_tree_get_first(l), t, n);\
                                                     &a->n!=NULL;\
                                                     a=psync_tree_element(psync_tree_get_next(&a->n), t, n))

#define psync_tree_for_each_element_call(l, t, n, c)\
  do {\
    psync_tree *___tmpa, *___tmpb;\
    t *___tmpe;\
    ___tmpa=psync_tree_get_first(l);\
    ___tmpe=psync_tree_element(___tmpa, t, n);\
    while (___tmpe){\
      __tmpb=psync_tree_get_next(__tmpa);\
      c(___tmpe);\
      ___tmpa=___tmpb;\
      ___tmpe=psync_tree_element(___tmpa, t, n);\
    }\
  } while (0)


typedef int (*psync_tree_compare)(const psync_tree *, const psync_tree *);

static inline long int psync_tree_height(psync_tree *tree){
  return tree?tree->height:0;
}

static inline psync_tree *psync_tree_get_first(psync_tree *tree){
  if (!tree)
    return tree;
  while (tree->left)
    tree=tree->left;
  return tree;
}

static inline psync_tree *psync_tree_get_last(psync_tree *tree){
  if (!tree)
    return tree;
  while (tree->right)
    tree=tree->right;
  return tree;
}

static inline psync_tree *psync_tree_get_next(psync_tree *tree){
  if (tree->right){
    tree=tree->right;
    while (tree->left)
      tree=tree->left;
    return tree;
  }
  else {
    while (tree->parent && tree==tree->parent->right)
      tree=tree->parent;
    return tree->parent;
  }
}

static inline psync_tree *psync_tree_get_prev(psync_tree *tree){
  if (tree->left){
    tree=tree->left;
    while (tree->right)
      tree=tree->right;
    return tree;
  }
  else {
    while (tree->parent && tree==tree->parent->left)
      tree=tree->parent;
    return tree->parent;
  }
}

psync_tree *psync_tree_get_add_after(psync_tree *tree, psync_tree *node, psync_tree *newnode);
psync_tree *psync_tree_get_add_before(psync_tree *tree, psync_tree *node, psync_tree *newnode);
psync_tree *psync_tree_get_added_at(psync_tree *tree, psync_tree *parent, psync_tree *newnode);

psync_tree *psync_tree_get_del(psync_tree *tree, psync_tree *node);


static inline psync_tree *psync_tree_get_add(psync_tree *tree, psync_tree *newnode, psync_tree_compare comp){
  psync_tree *el;
  if (!tree)
    return psync_tree_get_add_after(tree, NULL, newnode);
  el=tree;
  while (1){
    if (comp(newnode, el)<0){
      if (el->left)
        el=el->left;
      else
        return psync_tree_get_add_before(tree, el, newnode);
    }
    else{
      if (el->right)
        el=el->right;
      else
        return psync_tree_get_add_after(tree, el, newnode);
    }
  }
}

static inline void psync_tree_add_after(psync_tree **tree, psync_tree *node, psync_tree *newnode){
  *tree=psync_tree_get_add_after(*tree, node, newnode);
}

static inline void psync_tree_add_before(psync_tree **tree, psync_tree *node, psync_tree *newnode){
  *tree=psync_tree_get_add_before(*tree, node, newnode);
}

static inline void psync_tree_added_at(psync_tree **tree, psync_tree *parent, psync_tree *newnode){
  *tree=psync_tree_get_added_at(*tree, parent, newnode);
}

static inline void psync_tree_del(psync_tree **tree, psync_tree *node){
  *tree=psync_tree_get_del(*tree, node);
}

static inline void psync_tree_add(psync_tree **tree, psync_tree *newnode, psync_tree_compare comp){
  *tree=psync_tree_get_add(*tree, newnode, comp);
}

#endif
