#ifndef PARSE_H
#define PARSE_H

#include "main.h"

/*
struct key_value {
    void *key;
    void *value;
};
typedef struct key_value key_value;
*/

char **parse(char *, int *);
key_value **parse_kvs(char **, int);
key_value *parse_kv(char *raw);
unsigned char char2unsigned(char *, int);

#endif
