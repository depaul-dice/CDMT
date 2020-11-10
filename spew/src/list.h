#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>

struct listnode {
    void *ele;
    struct listnode *next;
};
typedef struct listnode listnode;

typedef struct {
    listnode *first;
    listnode *last;
    int size;
} list_t;

list_t *init_list(void);
int append_list(void *, list_t *);
void *pop_list(list_t *);
void destroy_list(list_t *);

#endif /* LIST_H */
