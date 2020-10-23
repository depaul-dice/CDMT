#ifndef HASHMAP_H
#define HASHMAP_H

#include "main.h"
#define WHOLE 1
#define HMONLY 0
/*
struct hash_node {
    fingerprint key;
    void *value;
    struct hash_node *next;
};
typedef struct hash_node hash_node;

struct bucket {
    int size;
    struct hash_node *top;
};
typedef struct bucket bucket;

struct hashmap {
    int size;
    int capacity;
    struct bucket **buckets;
    int *history;
};
typedef struct hashmap hashmap;
*/
/* returns 0 for success, returns 1 for failure */
hashmap *hashmap_create(int size, void (*destroy_value) (void *));

int hashmap_insert(hashmap *hm, fingerprint key, void *value);

/* 
 * returns the value if it finds the value with the key 
 * if it doesn't returns NULL
 */
void *hashmap_search(hashmap *hm, fingerprint key);

/* 
 * delete the node with key
 * returns 0 for success, 1 for failure (did not find the key) 
 */
int hashmap_delete(hashmap *hm, fingerprint key, int option);

/* counts how many buckets are destroyed */
int hashmap_destroy(hashmap *hm, int option);

void hashmap_print(hashmap *hm);

#endif /* HASHMAP_H */
