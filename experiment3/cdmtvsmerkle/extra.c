#include "main.h"

int verbosity = POS_VERBOSE;

struct r_value {
    Mtree *mt;
    int total;
    int changed;
};

int getline_(char *lineptr, size_t *n, FILE *stream) {
    static char line[256];
    char *ptr;
    unsigned len; 
   if(!lineptr || !n) {
        errno = EINVAL;
        return -1;
    }

    if(ferror(stream) || feof(stream))
        return -1;
    fgets(line, 256, stream);

    ptr = strchr(line, '\n');
    if(ptr)
        *ptr = '\0';
    else 
        return -1;
    len = strlen(line);
    if((len + 1) < 256) {
        //ptr = realloc(*lineptr, 256);
        
        //if(!ptr) return -1;
        //*lineptr = ptr;
        *n = 256;
        strcpy(lineptr, line);
        return len;
    } else {
        return -1;
    }
}

void pos_log(int level, const char *fmt, ...) {
    va_list ap;
    char msg[1024];
    if((level & 0xff) < verbosity) 
        return;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);
    fprintf(stdout, "%s", msg);
}

struct r_value *iteration_(char path[], Mtree *old);

int main(int argc, char **argv) {
    verbosity = POS_VERBOSE;

    Mtree *mt = NULL;
    int total_leaves = 0;
    int dif_leaves = 0;

    if(argc == 1) {
        fprintf(stderr, "Usage: %s dirname\n", argv[0]);
        return EXIT_FAILURE;
    }
    char filename_qfd[PATH_MAX];

    //FILE *fp = fopen("nodescompared.csv", "w");
    //fclose(fp);
    FILE *fp = NULL;
    strcpy(filename_qfd, "/home/yutan/cdmt/order/"); 
    strncat(filename_qfd, argv[1], PATH_MAX);
    strcat(filename_qfd, ".txt");
    DEBUG("filename for order: %s\n", filename_qfd);
    fp = fopen(filename_qfd, "r");
    if(!fp) {fprintf(stderr, "failed to open %s\n", filename_qfd); exit(1);}
    char *file = malloc(PATH_MAX); 
    
    if(!file) {perror("malloc failed"); exit(1);}
    size_t len = 0;
    while(getline_(file, &len, fp) != -1) {
        //printf("file: %s\n", file);
        strcpy(filename_qfd, "/home/yutan/cdmt/tmp_spew/");
        strncat(filename_qfd, argv[1], PATH_MAX);
        strncat(filename_qfd, "/", 1);
        file[strlen(file) - 4] = '\0';
        printf("file: %s\n", file);
        strncat(filename_qfd, file, strlen(file));
        strncat(filename_qfd, ".data.bk", strlen(".data.bk"));
        struct r_value *rv = iteration_(filename_qfd, mt);
        mt = rv->mt;
        total_leaves += rv->total;
        dif_leaves += rv->changed;
        free(rv);
    }
 
    Mtree_destroy(mt);
    free(file);
    fclose(fp);
   
    return 0;
}

struct r_value *iteration_(char path[], Mtree *old) {
    struct r_value *rv = malloc(sizeof(struct r_value));
    int mt_leaves_change = -1;
    int mt_total_leaves = -1;
    int trash = -1;

    char **parsed = parse(path, &mt_total_leaves);
    key_value **leaves = parse_kvs(parsed, mt_total_leaves);
    Mtree *current_mt = Mtree_create(leaves, mt_total_leaves, &trash);
    if(old) {
        assert(old);
        mt_leaves_change = Mtree_cmp_leaves_cnt(old, current_mt);
    }

    for(int i = 0; i < mt_total_leaves; i++) {
        free(parsed[i]);
        free(leaves[i]->key);
        free(leaves[i]->value);
        free(leaves[i]);
    }
    free(parsed); 
    free(leaves);
    Mtree_destroy(old);
    rv->total = mt_total_leaves;
    rv->changed = mt_leaves_change;
    rv->mt = current_mt;
    return rv;
}




    

