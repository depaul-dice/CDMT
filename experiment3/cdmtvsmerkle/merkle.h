#ifndef MERKLE_H
#define MERKLE_H

#include "main.h"
#define NUM_CHILDREN 4
Mtree *Mtree_create(key_value **, int, int *);
int Mtree_cmp(Mtree *, Mtree *, int *, int *);
Mnode *hash_Mtree(int *, key_value **, int, int *);
void Mtree_write(char *, Mtree *);

int count_num_nodes(Mnode *current);
int recursive_count(Mnode *a, Mnode *b, char a_old);
char *Merkle_meta_write(Mtree *mtree);
Mnode *Merkle_meta_restore(char *path);
void Mtree_destroy(Mtree *mtree);
void Mnode_destroy(Mnode *);

#endif
