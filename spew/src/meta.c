
#include "meta.h"

/*
typedef struct {
    size_t size;
    size_t offset;
    char name[PATH_MAX];
} file_t;
*/

list_t *init_metadata(void) {
    list_t *rv = init_list();
    return rv;
}

int add_metadata(char *name, size_t size, size_t offset, list_t *metalist) {
    file_t *file = malloc(sizeof(file_t));
    strncpy(file->name, name, strlen(name) + 1);
    file->size = size;
    file->offset = offset;
    if(append_list(file, metalist)) {
        fprintf(stderr, "appending failed\n");
        return 1;
    }
    return 0;
}

int write_metadata(list_t *metalist, char dst[]) {
    file_t *curr_file;
    FILE *fp = fopen(dst, "w");
    if(!fp) {
        fprintf(stderr, "opening the write file failed\n");
        return 1;
    }

    while((curr_file = pop_list(metalist))) {
        fprintf(fp, "%lu %lu %s\n", curr_file->offset, curr_file->size, curr_file->name);
        free(curr_file);
    }
    fclose(fp);
    return 0;
}

file_t *pop_metadata(list_t *metalist) {
    file_t *file = pop_list(metalist);
    return file;
}

void destroy_metadata(list_t *metalist) {
    destroy_list(metalist);
}

void print_metadata(list_t *metalist) {
    fprintf(stderr, "\nprinting meta\n");
    fprintf(stderr, "size: %d\n", metalist->size);
    listnode *node = metalist->first;
    while(node) {
        fprintf(stderr, "name: %s\n", ((file_t *)(node->ele))->name);
        node = node->next;
    }
}
