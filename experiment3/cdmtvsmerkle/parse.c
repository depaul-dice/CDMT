#include "parse.h"

char **parse(char *path, int *count) {

    FILE *fp = fopen(path, "r");
    if(!fp) {
        fprintf(stderr, "couldn't open file: %s\n", path);
        exit(1);
    }
    size_t characters = -1; /* this is MAX_SIZE */
    char *buffer = NULL;
    size_t bufsize = 32;
    int i = 0;
    char ** rv = malloc(sizeof(char *) * (1 << 7));
    while((characters = getline(&buffer, &bufsize, fp)) != -1) {
        char *raw = strtok(buffer, "]\n");
        do {
            rv[i] = malloc(strlen(raw) + 1);
            strncpy(rv[i++], raw, strlen(raw)+1);
            if(!(i % 128)) rv = realloc(rv, (i + (128)) *sizeof(char *)); 
            raw = strtok(NULL, "]\n");
        } while(raw);
    } 
    rv = realloc(rv, i * sizeof(char *));
    *count = i;

    free(buffer); 
    fclose(fp);
    return rv; 
}

key_value **parse_kvs(char **parsed, int count) {
    key_value **rv = malloc(sizeof(key_value *) *count);
    for(int i = 0; i < count; ++i){
        rv[i] = parse_kv(parsed[i]);
    }

    return rv;
}

key_value *parse_kv(char *raw) {
    /* 
     * format:
     * ["fingerprint"], value
     */
    char *process = NULL;
    int i = 0;
    key_value *rv = malloc(sizeof(key_value));
    rv->key =(void *)malloc(sizeof(fingerprint));
    rv->value = (void *)malloc(sizeof(int));
    for(process = strtok(raw, "\","); process; process = strtok(NULL, "\",")) {
        if(!memcmp(",[", process, sizeof(",[")) || !memcmp("[", process, sizeof("[")) || !memcmp("[[", process, sizeof("[[")))
            continue;
        if(!i++) {
            for(int j = 0; j < FP_SIZE; j++) {
                (*((fingerprint *)rv->key))[j] = char2unsigned(process, 2*j);
            }
        } else {
            int tmp = atoi(process);
            memcpy(rv->value, &tmp, sizeof(int));
        }
    }
    return rv;
}

unsigned char char2unsigned(char *process, int j) {
    int tmp = j;
    unsigned char rv = 0;
    while(tmp - j < 2) {
        if(islower(process[tmp])) {
            rv *= 16;
            rv += process[tmp++] - 97 + 10;
        } else {
            if(48 > process[tmp] || process[tmp] > 71) {
                printf("/%c /%s\n", process[tmp], process);
                exit(1);
            } 

            rv *= 16;
            rv += process[tmp++] - 48;
        }
    }
    assert(rv >= 0 && rv < 256);

    return rv;
}


