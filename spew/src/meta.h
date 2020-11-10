#ifndef META_H
#define META_H

#include <string.h>
#include "list.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif /* PATHMAX */

typedef struct {
    char name[PATH_MAX];
    size_t size;
    size_t offset;
} file_t;

list_t *init_metadata(void);
int add_metadata(char *name, size_t size, size_t offset, list_t *metalist);
int write_metadata(list_t *metalist, char dst[]);
file_t *pop_metadata(list_t *metalist);
void destroy_metadata(list_t *metalist);
void print_metadata(list_t *metalist);

#endif /* META_H */
