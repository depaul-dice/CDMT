#include "node.h"
#define getbit(from, which) ((from >> which) & 0x01) 

void node_set_all(node *curr, node **children, int height, int type, int num_children) {
    curr->children = children;
    curr->height = height;
    curr->type = type;
    curr->num_children = num_children;
    return;
}

node *node_create_all(node **children, int height, int type, int num_children) {
    node *rv = malloc(sizeof(node));
    if(!rv) {
        perror("not enough memory!\n");
    }
    node_set_all(rv, children, height, type, num_children);
    for(int i = 0; i < FP_SIZE; i++) {
        rv->fp[i] = 0;
    }
    return rv;
}

int node_add_children(int num, node *curr, node **children) {
    curr->children = realloc(curr->children, (curr->num_children + num) * sizeof(node *));
    if(!curr->children)
        return 1;

    for(int i = curr->num_children; i < (curr->num_children + num); i++)
        curr->children[i] = children[i];

    return 0;
}

void node_setfp(node* curr, fingerprint fp) {
    memcpy(curr->fp, fp, FP_SIZE);
}

void node_destroy(void *curr) {
    node *tmp = (node *)curr;
    if(!tmp){ 
        perror("this does not look good...\n");
        return;
    }
             
    if(tmp->children) {
        free(tmp->children);
        tmp->children = NULL;
    }
    free(tmp);
    return;
}

int check_sha1(node *curr, node *prev) {
    SHA_CTX ctx;
	SHA1_Init(&ctx);
	fingerprint fp; 

	SHA1_Update(&ctx, curr->fp, 20);
    if(prev) {
        SHA1_Update(&ctx, prev->fp, 20);
        SHA1_Final(fp, &ctx);
        unsigned char b = fp[19];
		if( !getbit(b, 0) && !getbit(b, 1)) {
            return 1;
        }
    } else {
    }
    return 0;
}

void free_data_(void * data) {
	if(data)
	    free(data);
    data = NULL;
	return;
}

int fp_cmp(fingerprint f1, fingerprint f2) {
    int count = 0;
    for(int i = 0; i < FP_SIZE; i++)
        if(f1[i] != f2[i]) count++;
    return count;
}

