#include "hashmap.h"

#define getbit(from, which) ((from >> which) & 0x01)
#define getlessigbyte(from, which) (which & 0xff)
/* 
 * you want to see the least 11 significant bits 
 * and mod with the size to get the position 
 */
/*
int hash_function_(fingerprint fp, int size) {
    unsigned it = -1;
    it = it & fp[19];
    it = it & (fp[18] << 8);
    return it % size;
}
*/

int hash_function_(fingerprint fp, int size) {
    int rv = 0;
    int it = 1;
    unsigned char b = fp[19];
    for(int i = 0; i < 8; i++) {
        if(getbit(b, i))
            rv += it;
        it *= 2;
    }
    b = fp[18];
    for(int i = 0; i < 8; i++) {
        if(getbit(b, i))
            rv += it;
        it *= 2;
    }
    return rv % size;
}

static void fp_write(FILE  *fp, fingerprint fp_){ 
    unsigned char num = 0;
    fprintf(fp, "\"");
    int first = -1;
    int second = -1;
    for(int i = 0; i < sizeof(fingerprint); ++i) {
        num = fp_[i]; 
        assert(num < 256 && 0 <= num);
        first = num/16;
        second = num%16;
        if(first > 9) first += 97 - 10; else first += 48;
        if(second > 9) second += 97 - 10; else second += 48;
        fprintf(fp, "%c%c", first, second);
    }
    fprintf(fp, "\"");
}

/* returns some pointer for success, returns NULL for failure */
hashmap *hashmap_create(int capacity, void (*destroy_value) (void *)) {
    if(capacity > (1 << 16)) {
        perror("size is too big!\n");
        return NULL;
    }

    hashmap *rv = malloc(sizeof(hashmap));
    if(!rv) {
        perror("not enough memory\n");
        return NULL;
    }
        
    rv->size = 0;
    rv->capacity = capacity;
    rv->buckets = calloc(capacity, sizeof(bucket *));
    if(!(rv->buckets)) {
        perror("not enough memory\n");
        free(rv);
        return NULL;
    }

    rv->history = malloc(sizeof(fingerprint) * 128);
    if(!(rv->history)) {
        perror("not enough memory\n");
        free(rv->buckets);
        free(rv);
        return NULL;
    }
    rv->destroy_value = destroy_value;
    return rv;
}

int hashmap_insert(hashmap *hm, fingerprint key, void *value) {
    assert(hm);

    int bucket_num = hash_function_(key, hm->capacity);
    assert(hm->buckets);
    if(!(hm->buckets[bucket_num])) {
        hm->buckets[bucket_num] = malloc(sizeof(bucket));
        /* create the->top */
        hm->buckets[bucket_num]->size = 1;

        hash_node *tmp = malloc(sizeof(hash_node));
        if(!tmp) { printf("not enough memory!"); exit(1);}
        memcpy(tmp->key, key, sizeof(fingerprint));
        /* value here is (node **) */
        tmp->value = value;
        tmp->next = NULL;
        
        hm->buckets[bucket_num]->top = tmp;
        
    } else {
        hash_node *current = hm->buckets[bucket_num]->top;
        hash_node *prev = NULL;
        while(current) {
            if(!memcmp(key, current->key, sizeof(fingerprint))) {
                //fp_write(stdout, current->key); printf(": %d\n", hm->size); 
                return 1;
            } 
            prev = current;
            current = current->next;
        }

        hash_node *tmp = malloc(sizeof(hash_node));
        if(!tmp) {printf("not enough memory!"); exit(1);}
        memcpy(tmp->key, key, sizeof(fingerprint));
        tmp->value = value;
        /* copying node */
        tmp->next = NULL;

        prev->next = tmp;
        hm->buckets[bucket_num]->size += 1;
    }

    memcpy(hm->history[hm->size++], key, sizeof(fingerprint));
    //fp_write(stdout, hm->history[hm->size - 1]); printf(": %d\n", hm->size); 
    if(hm->size % 128 == 0) {
        hm->history = realloc(hm->history, sizeof(fingerprint) * (hm->size + 128));
    }
    return 0;
}

/* 
 * returns the value if it finds the value with the key 
 * if it doesn't returns NULL
 */
void *hashmap_search(hashmap *hm, fingerprint key) {
    int bucket_num = hash_function_(key, hm->capacity);
    if(!(hm->buckets[bucket_num]) || !((hm->buckets)[bucket_num]->top))
       return NULL;

    hash_node *node_ = ((hm->buckets)[bucket_num])->top;
    while(node_){
        if(!memcmp(node_->key, key, sizeof(fingerprint)))  
            return (node_->value);
        node_ = node_->next;
    }
    return NULL;
}

/* 
 * delete the node with key
 * returns 0 for success, 1 for failure (did not find the key) 
 */
int hashmap_delete(hashmap *hm, fingerprint key, int option) {
    int bucket_num = hash_function_(key, hm->capacity);
    if(!hm || !(hm->buckets) || !(hm->buckets)[bucket_num] || !((hm->buckets)[bucket_num]->top)){
        return 1;
    }
    hash_node *node = hm->buckets[bucket_num]->top;
    hash_node *prev = NULL;
    while(node) {
        if(!memcmp((node->key), key, sizeof(fingerprint))) {
            if(prev) {
                prev->next = node->next;
                hash_node *tmp = node;
                hm->destroy_value(tmp->value);
                free(tmp);
                node = node->next;
            } else {
                hm->buckets[bucket_num]->top = node->next;
                hash_node *tmp = node;
                node = node->next;
                if(option)
                    hm->destroy_value(tmp->value);
                free(tmp);
            }
            ((hm->buckets)[bucket_num]->size) -= 1;
            //printf("hm->buckets[%d]->size: %d\n", bucket_num, hm->buckets[bucket_num]->size);
            assert((hm->buckets)[bucket_num]->size >= 0);
            if(!((hm->buckets)[bucket_num]->size)) {
                /* free the bucket */
                free(hm->buckets[bucket_num]);
                hm->buckets[bucket_num] = NULL;
            }

            (hm->size)-= 1;
            assert(hm->size >= 0);
            return 0;
        } else {
            prev = node;
            node = node->next;
        }
    }
    return 1;
}
/*
void bucket_destroy(bucket *bkt, void (*destroy_value) (void **), int option) {
    if(!bkt) {
        return;
    }
    hash_node *node = bkt->top;
    
    hash_node *tmp = NULL;
    while(node) {
        tmp = node;
        node = node->next;
        if(option && tmp->value)
            destroy_value((tmp->value));
        free(tmp->value);
        free(tmp);
    }
    free(bkt);
    bkt = NULL;
    return;
}
*/
/* counts how many buckets are destroyed */
/*
void hashmap_destroy(hashmap *hm, int option) {

    for(int i = 0; i < hm->size; i++) {
        if(hm->buckets[hm->history[i]])
            bucket_destroy(hm->buckets[hm->history[i]], hm->destroy_value, option);
    }
    free(hm->buckets);
    free(hm->history);
    free(hm);
    hm = NULL;
}
*/

int hashmap_destroy(hashmap *hm, int option) {
    if(!hm) return 1;
    int delete_cnt = hm->size;
    for(int i = 0; i < delete_cnt; i++) {
        //fp_write(stdout, hm->history[i]); printf(": %d\n", hm->size);
        if(hashmap_delete(hm, hm->history[i], option)) return 1;        
    }
    free(hm->buckets);
    free(hm->history);
    assert(!(hm->size));
    free(hm);
    return 0;
}
void bucket_print(bucket *bkt) {
    if(!bkt) {
        fprintf(stdout, "nothing in here\n\n");
        return;
    }
    
    fprintf(stdout, "size: %d\n", bkt->size);
    hash_node *node = bkt->top;
    while(node) {
        fprintf(stdout, "key: ");
        fp_write(stdout, node->key);
        node = node->next;      
    }
    return;
}

void hashmap_print(hashmap *hm) {
    if(!hm)
        return;
    for(int i = 0; i < hm->size; i++) {
        fprintf(stdout, "i: %d, hm->history[i]: ", i);
        fp_write(stdout, hm->history[i]);
    }
    return;
}
