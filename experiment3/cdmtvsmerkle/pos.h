#ifndef POS_H
#define POS_H

#include "main.h"

/*
struct hm_root {
    hashmap *hm;
    node *root;
};
typedef struct hm_root hm_root;
*/

node *create_POS(key_value **leaves, int count, hashmap *hm, int *node_difference, int *total_nodes);

void insert_nodes(node *root, hashmap *hm);
void POS_write(hashmap *hm, node *root, int i);

void subtree_destroy(node *current);

#endif /* POS_H */
