
#ifndef MAIN_H 
#define MAIN_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>

#include "meta.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif /* PATHMAX */

size_t recursive_write(char *dirname, FILE *fp_w, size_t offset, list_t *metalist);

#endif /* MAIN_H */
