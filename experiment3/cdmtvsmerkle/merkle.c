#include "merkle.h"

int root_cmp(Mnode *root_a, Mnode *root_b);

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

Mnode *Mnode_create(fingerprint fp, Mnode **children, int height) {
    Mnode *rv = malloc(sizeof(Mnode));
    rv->children = children;
    rv->height = height;

    return rv;
}

Mnode *hash_Mtree(int *height, key_value **leaves, int leaves_count, int *mt_total_nodes) {
    int total_nodes_local = 0;
    Queue *queue = queue_new();
    Mnode *current = NULL;
    /* taking care of leaves */
    for(int i = 0; i < leaves_count; i++) {
        current = Mnode_create(*(fingerprint *)(leaves[i]->key), NULL, 0);
        total_nodes_local++;
        memcpy(current->fp, *(fingerprint *)(leaves[i]->key), sizeof(fingerprint));
        queue_push(queue, current);
    }
    Mnode **children = malloc(sizeof(Mnode *) * NUM_CHILDREN);
    SHA_CTX ctx;
    Mnode *curr_child = NULL;
    Mnode *curr_parent = NULL;
    int count_children = 0;
    while((curr_child = queue_pop(queue))) {
        if(!count_children) {
            children[count_children] = curr_child;
            count_children++;
            continue;
        }

        if(curr_child->height > children[count_children - 1]->height) {
            curr_parent = Mnode_create(NULL, children, (children[0])->height + 1);
            total_nodes_local++;
            SHA1_Init(&ctx);
            for(int i = 0; i < count_children; i++) {
                SHA1_Update(&ctx, children[i]->fp, sizeof(fingerprint));
            }
            SHA1_Final(curr_parent->fp, &ctx);
            for(int i = count_children; i < NUM_CHILDREN; i++) {
                curr_parent->children[i] = NULL;
            }
            queue_push(queue, curr_parent);
            children = malloc(sizeof(Mnode *) *NUM_CHILDREN);
            assert(children);
            children[0] = curr_child;
            count_children = 1;
            continue;
        }

        if(count_children == NUM_CHILDREN - 1) {
            children[count_children] = curr_child;
            curr_parent = Mnode_create(NULL, children, (children[0])->height + 1);
            total_nodes_local++;
            SHA1_Init(&ctx);
            for(int i = 0; i < count_children; i++) {
                SHA1_Update(&ctx, children[i]->fp, sizeof(fingerprint));
            }
            SHA1_Final(curr_parent->fp, &ctx);
            queue_push(queue, curr_parent);
            children = malloc(sizeof(Mnode *) * NUM_CHILDREN);
            count_children = 0;
            continue;
        }

        children[count_children] = curr_child;
        count_children++;
    }

    Mnode *root = NULL;
    assert(count_children);
    if(count_children == 1) {
        root = children[0];
        free(children);
    } else {
        assert(children);
        root = Mnode_create(NULL, children, children[0]->height + 1);
        total_nodes_local++;
        for(int i = count_children; i < NUM_CHILDREN; i++)
            children[i] = NULL;
        SHA1_Init(&ctx);
        for(int i = 0; i < count_children; i++) {
            SHA1_Update(&ctx, children[i]->fp, sizeof(fingerprint));
        }
        SHA1_Final(root->fp, &ctx);
        queue_free(queue, free_data_);
    }
    *mt_total_nodes = total_nodes_local;
    *height = root->height;
    return root;
}

Mtree *Mtree_create(key_value **leaves, int leaves_count, int *mt_total_nodes) {
    Mtree *mtree = malloc(sizeof(Mtree));
    mtree->root = NULL;
    mtree->height = 0;
    mtree->root = hash_Mtree(&(mtree->height), leaves, leaves_count, mt_total_nodes);
    return mtree;
}

int Mtree_cmp(Mtree *a, Mtree *b, int *mt_total_change, int *mt_internal_change) {
    /* a is old, b is current */
    int count =  0;
    count = root_cmp(a->root, b->root);
    *mt_total_change = count;
    return count;
}

int count_num_leaves(Mnode *curr) {
    int cnt = 0;
    if(!curr) return 0;
    if(curr->height == 0) {
        return 1;
    } else {
        for(int i = 0; i < NUM_CHILDREN; i++)
            cnt+= count_num_leaves(curr->children[i]);
    }
    return cnt;
}

int recursive_count_num_leaves(Mnode *a, Mnode *b, char a_old) {
    int count = 0;
    Mnode *old = NULL;
    Mnode *new = NULL;
    if(a_old) {
        old = a; new = b;
    } else { 
        old = b; new = a;  
    }
    if(!new)
        return count;
    if(!old) 
        return count_num_leaves(new); 

    if(memcmp(old->fp, new->fp, sizeof(fingerprint))) {
        if(old->children && new->children) {
            for(int i = 0; i < NUM_CHILDREN; i++){
                count += recursive_count_num_leaves(a->children[i], b->children[i], a_old);
            }
        } else if (!(old->children) && new->children) {
            for(int i = 0; i < NUM_CHILDREN; i++)
                count += count_num_leaves(new->children[i]);
        } else if (old->children && !(new->children)) {
        } else {
            if(new->height == 0) count++; 
        }
    }
    return count;
}

int root_cmp_leaves_cnt(Mnode *root_a, Mnode *root_b) {
    /* a is old, b is new */
    assert(root_a && root_b);
    int count = 0;

    Mnode *tmp_high = (root_a->height > root_b->height) ? root_a : root_b;
    Mnode *tmp_low = (root_a->height > root_b->height) ? root_b : root_a;
    char a_old = -1;
    if(tmp_high == root_a)
        a_old = 1;
    else 
        a_old = 0;

    while(tmp_high->height > tmp_low->height) {
        assert(tmp_high -> children);
        count++;
        for(int i = 1; i < NUM_CHILDREN; i++) {
            if(tmp_high->children[i] && (!a_old))
                count += count_num_leaves(tmp_high->children[i]);
            else
                break;
        }
        tmp_high = tmp_high->children[0];
    }
    assert(tmp_high);
    count += recursive_count_num_leaves(tmp_high, tmp_low, a_old); 

    return count;
}

int Mtree_cmp_leaves_cnt(Mtree *a, Mtree *b) {
    int count = 0;
    count = root_cmp_leaves_cnt(a->root, b->root);
    return count;
}

void Mtree_write(char *path, Mtree *mtree) {
    FILE *fp_ = fopen(path, "w");
    Mnode *root = mtree->root;
    Mnode *element = NULL;
    Queue *stack = queue_new();
    queue_push(stack, root);
    while((element = queue_pop(stack))) {
        fprintf(fp_, "this is new node\n");
        fprintf(fp_, "self: ");
        fp_write(element->fp, fp_); 
        fprintf(fp_, "\nheight: %d\n", element->height);
        if(element->children) {
            for(int i = 0; i < NUM_CHILDREN; i++){
                if(!element->children[i])
                    fprintf(fp_, "children[%d]: NULL\n", i);
                else {
                    fprintf(fp_, "children[%d]: ", i);
                    fp_write(element->children[i]->fp, fp_);
                    fprintf(fp_, "\n");
                }
            }
        }
        fprintf(fp_, "\n");
        if(element->children) {
            for(int i = 0; i < NUM_CHILDREN; i++) {
                if(element->children[i]) 
                    queue_push(stack, element->children[i]);
                else 
                    break;
            }
        }
        
    }
    assert(!queue_size(stack));
    queue_free(stack, free_data_);
    fclose(fp_);
}

int root_cmp(Mnode *root_a, Mnode *root_b) {
    /* a is old, b is new */
    assert(root_a && root_b);
    int count = 0;

    Mnode *tmp_high = (root_a->height > root_b->height) ? root_a : root_b;
    Mnode *tmp_low = (root_a->height > root_b->height) ? root_b : root_a;
    char a_old = -1;
    if(tmp_high == root_a)
        a_old = 1;
    else 
        a_old = 0;

    while(tmp_high->height > tmp_low->height) {
        assert(tmp_high -> children);
        count++;
        for(int i = 1; i < NUM_CHILDREN; i++) {
            if(tmp_high->children[i] && (!a_old))
                count += count_num_nodes(tmp_high->children[i]);
            else
                break;
        }
        tmp_high = tmp_high->children[0];
    }
    assert(tmp_high);
    count += recursive_count(tmp_high, tmp_low, a_old); 

    return count;
}

int count_num_nodes(Mnode *current) {
    if(!current)
        return 0;

    int count = 1;
    if(current->children) {
        for(int i = 0; i < NUM_CHILDREN; i++) {
            if(current->children[i])
                count += count_num_nodes(current->children[i]);
            else
                break;
        }
    }
    return count;
}

int recursive_count(Mnode *a, Mnode *b, char a_old) {
    int count = 0;
    Mnode *old = NULL;
    Mnode *new = NULL;
    if(a_old) {
        old = a; new = b;
    } else { 
        old = b; new = a;  
    }
    if(!new)
        return count;
    if(!old) 
        return count_num_nodes(new); 

    if(memcmp(old->fp, new->fp, sizeof(fingerprint))) {
        count++;
        if(old->children && new->children) {
            for(int i = 0; i < NUM_CHILDREN; i++){
                count += recursive_count(a->children[i], b->children[i], a_old);
            }
        } else if (!(old->children) && new->children) {
            for(int i = 0; i < NUM_CHILDREN; i++)
                count += count_num_nodes(new->children[i]);
        } else if (old->children && !(new->children)) {
            /*
            for(int i = 0; i < NUM_CHILDREN; i++)
                count += count_num_nodes(a->children[i]);
            */
        }
    }
    return count;
}

int recursive_count_all(Mnode *a, Mnode *b) {
    int count = 0;
    if(!a && !b)
        return count;
    if(!a || !b) 
        return (a) ? count_num_nodes(a):count_num_nodes(b); 
    if(memcmp(a->fp, b->fp, sizeof(fingerprint))) 
        count++;
    if(a->children && b->children) {
        for(int i = 0; i < NUM_CHILDREN; i++) {
            count += recursive_count_all(a->children[i], b->children[i]);
        }
    } else if (!(a->children) && b->children) {
        for(int i = 0; i < NUM_CHILDREN; i++)
            count += count_num_nodes(b->children[i]);
    } else if (a->children && !(b->children)) {
        for(int i = 0; i < NUM_CHILDREN; i++)
            count += count_num_nodes(a->children[i]);
    }
    return count;
}
   

void M_write_node(Mnode *node, FILE *fp) {
    fprintf(fp, "fp:");
    for(int i = 0; i < FP_SIZE; ++i){
        fprintf(fp, "%u|", node->fp[i]);
    }
    fprintf(fp, "\n");
    if(node->children) {
        for(int i = 0; i < NUM_CHILDREN; ++i) {
            fprintf(fp, "children[%d]->fp:", i);
            if(node->children[i]) {
                fp_write(node->children[i]->fp, fp);
                fprintf(fp, "\n");
            } else {
                fprintf(fp, "0\n");
            }
        }
    } 
    fprintf(fp, "height:%d\n", node->height);
    fprintf(fp, "\n");
    return;
}

int is_regular(char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char *Merkle_meta_write(Mtree * mtree) {
    Mnode *root = mtree->root;
    Queue *queue = queue_new();
    queue_push(queue, root);

    char *path = calloc(100, sizeof(char));
    strcpy(path, "");
    strcpy(path, "./Merklemeta/");

    char *tmp = malloc(sizeof(char) * 10);
    for(int i = 0; i < FP_SIZE; i++) {
        sprintf(tmp, "%u|", root->fp[i]);
        path = strcat(path, tmp);
    }
    
    free(tmp);

    path = strcat(path,  ".dat");
    if(is_regular(path)) {
        printf("same tree seems to exist already!");
        return NULL;
    }
    printf("path for metadata: %s", path);
    FILE *fp = fopen(path, "w");
    Mnode *current = NULL;
    while(queue_size(queue)) {
        current = (Mnode *)queue_pop(queue);
        assert(current);
        if(current->children) {
            for(int i = 0; i < NUM_CHILDREN; ++i){
                if(current->children[i])
                    queue_push(queue, current->children[i]);
            }
        }
        M_write_node(current, fp);
    } 
    fclose(fp);
    queue_free(queue, free_data_);
    return path;
}
/*
Mnode *parse_M_nodes(FILE *fp) {
    char *buffer = NULL;
    size_t bufsize = 32;
    size_t characters = -1;
    hashmap *hashmap_ = hashmap_create(512);

    Mnode **children = calloc(NUM_CHILDREN, sizeof(Mnode *));
    Mnode *current = M_node_create(NULL, children, -1, -1);
    Mnode *root = current;
    while((characters = getline(&buffer, &bufsize, fp)) != SIZE_MAX) {
        char *prefix = strtok(buffer, ":");
        if(!memcmp(prefix, "height", sizeof("height"))) {
            char *height = strtok(NULL, "\n");
            current->height = atoi(height);
        } else if(!memcmp(prefix, "leaf", sizeof("leaf"))) {
            char *leaf = strtok(NULL, "\n");
            current->leaf = atoi(leaf);
        } else if(strstr(prefix, "fp")) {
            char *rest = strtok(NULL, "\n");
            fingerprint fp;
            parse_fp(rest, fp);
            if(!memcmp(prefix, "fp", sizeof("fp"))) {
                memcpy(current->fp, fp, sizeof(fingerprint));
                child_index *ci = NULL;
                if(memcmp(fp, root->fp, sizeof(fingerprint))) {
                    assert((ci = (child_index *)hashmap_search(hashmap_, fp)));

                    ((Mnode *)(ci->parent))->children[ci->index] = current;
                }
            } else if(!memcmp(prefix, "children[0]->fp", sizeof("children[0]->fp"))) {
                child_index *ci = malloc(sizeof(child_index));
                ci->index = 0;
                ci->parent = (void *)current;
                assert(!hashmap_insert(hashmap_, fp, ci));
            } else if(!memcmp(prefix, "children[1]->fp", sizeof("children[1]->fp"))) {
                child_index *ci = malloc(sizeof(child_index));
                ci->index = 1;
                ci->parent = (void *)current;
                assert(!hashmap_insert(hashmap_, fp, ci));
            } else if(!memcmp(prefix, "children[2]->fp", sizeof("children[2]->fp"))) {
                child_index *ci =malloc(sizeof(child_index));
                ci->index = 2;
                ci->parent = (void *)current;
                assert(!hashmap_insert(hashmap_, fp, ci));
            } else if(!memcmp(prefix, "children[3]->fp", sizeof("children[3]->fp"))) {
                child_index *ci = malloc(sizeof(child_index));
                ci->index = 3;
                ci->parent = (void *)current;
                assert(!hashmap_insert(hashmap_, fp, ci));
            } else {
                printf("THIS SHOULD NOT HAPPEN");
            }
        } else {
            assert(!memcmp(prefix, "\n", sizeof("\n")));

            children = calloc(NUM_CHILDREN, sizeof(M_node *));
            current = M_node_create(NULL, children, -1, -1);
        }
        free(buffer);
        buffer = NULL; 
    }
    hashmap_destroy(hashmap_);
    return root;
}
*/
/*
Mnode *Merkle_meta_restore(char *path) {
    printf("restore path: %s\n", path); 
    assert(is_regular(path)); 
    FILE *fp = fopen(path, "r");
    Mnode *root = parse_M_nodes(fp);
    fclose(fp);
    return root;
}
*/
void Mtree_destroy(Mtree *mtree) {
    Mnode_destroy(mtree->root);
    free(mtree);
}

void Mnode_destroy(Mnode *current) {
    assert(current);
    if(current->height > 0) {
        for(int i = 0; i < NUM_CHILDREN; i++) {
            if(current->children[i])
                Mnode_destroy(current->children[i]);
        }
        free(current->children);
    }
    free(current);
}
