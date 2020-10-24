#ifndef NODE_H_
#define NODE_H_

#include "main.h"
#define ID_COUNT 512
#define INTERNAL 0
#define LEAF 1
#define ROOT 2
#define EXTERNAL 3

/*
struct node {
    fingerprint fp;
    struct node **children;
    int height;
    int type;
    int num_children;
};
typedef struct node node;
*/

node *node_create_all(node **children, int height, int type, int num_children);
node *( *node_create_leaves)(node ** , int, int, int);
node *( *node_create_cut)(node **, int, int, int);
node *( *node_create_first)(node **, int, int, int);

int node_add_children(int num, node *curr, node **children);

void node_setfp(node* curr, fingerprint fp);

void node_destroy(void *curr);

int check_sha1(node *curr, node *prev);

void free_data_(void * data);

#endif /* NODE_H_ */
