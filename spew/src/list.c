
#include "list.h"
/*
struct listnode {
    void *ele;
    struct listnode *next;
};
typedef struct listnode listnode;

typedef struct {
    listnode *first;
    listnode *last;
    int size;
} list;
*/

list_t *init_list(void) {
    list_t *rv = malloc(sizeof(list_t));
    rv->first = NULL;
    rv->last = NULL;
    rv->size = 0;
    return rv;
}

int append_list(void *ele, list_t *list) {
    list->size++;
    listnode *curr = malloc(sizeof(listnode));
    if(!curr) return 1;
    curr->ele = ele;
    curr->next = NULL;
    if(!(list->first)) {
        list->first = curr;
    } 
    if(list->last) {
        list->last->next = curr;
        list->last = curr;
    } else {
        list->last = curr;
    }
    return 0;
}

void *pop_list(list_t *list) {
    if(list->size <= 0) {
        fprintf(stderr, "size nonpositive\n");
        return NULL;
    }
    listnode *tmp = list->first;
    void *rv = tmp->ele;
    list->first = tmp->next;
    list->size--;
    free(tmp);
    return rv;
}

void destroy_list(list_t *list) {
    while(list->size > 0) {
        fprintf(stderr, "size: %d\n", list->size);
        pop_list(list);
    }
    free(list);
}
