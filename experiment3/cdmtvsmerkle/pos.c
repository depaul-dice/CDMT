#include "pos.h"

static void fp_write(fingerprint fp_, FILE *fp) {
    unsigned char num = 0;
    fprintf(fp, "\"");
    int first = -1;
    int second = -1;
    for(int i = 0; i < FP_SIZE; ++i) {
        num = fp_[i];
        assert(num < 256 && 0 <= num);
        first = num/16;
        second = num%16;
        if(first > 9) first += 97 - 10; else first += 48;
        if(second > 9) second += 97 - 10; else second += 48;
        fprintf(fp, "%c%c", first, second);
    }
    fprintf(fp, "\",");
}

node *create_POS(key_value **leaves, int count, hashmap *hm, int *node_difference, int *total_nodes) {

    int total_nodes_local = 0;
    int node_difference_local = 0;

    Queue *elements = queue_new();
    Queue *new_entries = queue_new();
    node *current = NULL;
    node *ptr = NULL;
    //printf("leaves\n");
    for(int i = 0; i < count; ++i) {
        current = node_create_all(NULL, 0, LEAF, -1);
        memcpy(current->fp, *(fingerprint *)(leaves[i]->key), sizeof(fingerprint));
        total_nodes_local += 1;
        if(hashmap_insert(hm, current->fp, current)) {
            //printf("happened to be the same LEAF\n");
            ptr = (node *)hashmap_search(hm, current->fp);
            /* free the current */
            node_destroy(current);
            current = ptr;
        } else {
            node_difference_local += 1;
        }
        
        queue_push(new_entries, current);
    }

    //printf("internal_nodes\n");
    Queue *tmp = NULL; 
    node *curr_parent = NULL;
    //node **children = NULL;
    node *curr_child = NULL;
    node *prev_child = NULL;
    int num_children = 0;
    SHA_CTX ctx;
    do {
        //printf("next level\n");
        assert(!queue_size(elements));
        tmp = elements;
        elements = new_entries;
        new_entries = tmp;
        curr_parent = NULL;
        curr_child = NULL;
        prev_child = NULL;
        num_children = 0;
        curr_parent = node_create_all(NULL, -1, INTERNAL, -1);
        curr_parent->children = calloc(32, sizeof(node *));
        SHA1_Init(&ctx);
        total_nodes_local += 1;
        if(!(curr_parent->children)) {
            perror("not enough memory!\n");
            exit(1);
        }

        while(queue_size(elements) > 0) {
            curr_child = queue_pop(elements);
            curr_parent->children[num_children++] = curr_child;
            if(num_children % 32 == 0) {
                curr_parent->children = realloc(curr_parent->children, sizeof(node *) * (num_children + 32));
                if(!(curr_parent->children)) {
                    perror("not enough memory\n");
                    exit(1);
                }
            }
            SHA1_Update(&ctx, curr_child->fp, FP_SIZE);

            if(check_sha1(curr_child, prev_child)) {
                SHA1_Final(curr_parent->fp, &ctx);
                curr_parent->children = realloc(curr_parent->children, sizeof(node *) * num_children);
                curr_parent->num_children = num_children;
                curr_parent->height = curr_parent->children[0]->height + 1;

                if(hashmap_insert(hm, curr_parent->fp, curr_parent)) {
                    //printf("happened to be the same INTERNAL\n");
                    assert(ptr = (node *)hashmap_search(hm, curr_parent->fp));
                    node_destroy(curr_parent);
                    curr_parent = ptr;                 
                } else {
                    node_difference_local += 1;
                }
                queue_push(new_entries, curr_parent);

                SHA1_Init(&ctx);
                curr_parent = node_create_all(NULL, -1, INTERNAL, -1);
                total_nodes_local += 1;
                curr_parent->children = calloc(32, sizeof(node *));
                if(!(curr_parent->children)) {
                    perror("not enough memory!\n");
                    exit(1);
                }
                num_children = 0;
                prev_child = NULL;
            } else {
                prev_child = curr_child;
            }
        }
        
        assert(!queue_size(elements));
        if(num_children) { 
            SHA1_Final(curr_parent->fp, &ctx);
            curr_parent->children = realloc(curr_parent->children, sizeof(node *) *num_children);
            curr_parent->num_children = num_children;
            assert(curr_parent->children);
            curr_parent->height = curr_parent->children[0]->height + 1;
            
            if(hashmap_insert(hm, curr_parent->fp, curr_parent)) {
                //printf("happened to be the same INTERNAL\n"); 
                assert(ptr = (node *)hashmap_search(hm, curr_parent->fp));
                node_destroy(curr_parent);
                curr_parent = ptr;
            } else {
                node_difference_local += 1;
            }
            queue_push(new_entries, curr_parent);
            num_children = 0;
        } else {
            node_destroy(curr_parent);
        }
    } while(queue_size(new_entries) > 1);

    if(queue_size(new_entries) != 1) {
        //printf("count: %d\n", count);
        if(!queue_size(new_entries)) {
            queue_free(elements, free_data_);
            queue_free(new_entries, free_data_);
            return NULL;
        }
        fprintf(stderr,"queue_size: %d\n", queue_size(new_entries));
        exit(1);
    }
    node *root = queue_pop(new_entries);
    root->type = ROOT;

    assert(!queue_size(elements) && !queue_size(new_entries));
    queue_free(elements, free_data_);
    queue_free(new_entries, free_data_);
    *total_nodes = total_nodes_local;
    *node_difference = node_difference_local;

    return root;
}

void insert_nodes(node *root, hashmap *hm) {
    Queue *queue = queue_new();
    queue_push(queue, root);
    node *current = NULL;
    while((current = queue_pop(queue))){
        if(current->children) {
            for(int i = 0; i < current->num_children; i++) {
                queue_push(queue, current->children[i]);
            }
        }
        hashmap_insert(hm, current->fp, current);
    }
    assert(!queue_size(queue));
    queue_free(queue, free_data_);
    return; 
}

void node_write(FILE *fp, node *current) {
    fp_write(current->fp, fp);
    fprintf(fp, "%d,", current->height);
    if(current->type == LEAF) {
        fprintf(fp, "LEAF,");
    } else if(current->type == INTERNAL) {
        fprintf(fp, "INTERNAL,");
    } else {
        fprintf(fp, "ROOT,");
    }
    fprintf(fp, "%d,", current->num_children);
    if(current->children) {
        for(int i = 0; i < current->num_children; ++i) {
            fp_write(current->children[i]->fp, fp);
        }
    }
    fprintf(fp, "\n");
}

void POS_write(hashmap *hm, node *root, int i) {    
    char cat [8];
    sprintf(cat, "%d", i);
    char path [128];

    strcpy(path, "/home/yuta2/Desktop/merkle_vs_pos/POS/independent/pos/posmeta/root");
    strncat(path, cat, strlen(cat) + 1);
    strncat(path, ".txt", strlen(".txt") + 1);
    //printf("path: %s\n", path);

    FILE *fp = fopen(path, "w");
    if(!fp){ printf("error opening the file %s\n", path); exit(1);}
    Queue *queue = queue_new();
    queue_push(queue, root);
    while(queue_size(queue) > 0) {
        node *current = queue_pop(queue);
        assert(current);
        if(!hashmap_insert(hm, current->fp, current)) {
            node_write(fp, current);
            for(int i = 0; i < current->num_children; ++i) {
                assert(current->children[i]);
                queue_push(queue, current->children[i]);
            }
        } else {
           fprintf(fp, "this part has already been written\n"); 
        }
    }
    queue_free(queue, free_data_);
    fclose(fp);
}

void subtree_destroy(node *current) {
    assert(current);
    if(current->children) {
        for(int i = 0; i < current->num_children; i++) {
            subtree_destroy(current->children[i]);
        }
    }
    free(current->children); 
    current->children = NULL;
    free(current);
}
