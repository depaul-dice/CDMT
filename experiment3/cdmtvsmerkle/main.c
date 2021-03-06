#include "main.h"

int verbosity = POS_VERBOSE;

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

int main(int argc, char **argv) {
    verbosity = POS_VERBOSE;

    //hashmap *hm = NULL;
    hm_mt *hmt = NULL;

    if(argc == 1) {
        fprintf(stderr, "Usage: %s dirname\n", argv[0]);
        return EXIT_FAILURE;
    }
    char filename_qfd[PATH_MAX];
    char old_path[PATH_MAX];
    old_path[0] = 0;

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
        hmt = iteration(filename_qfd, hmt);
        strncpy(old_path, filename_qfd, PATH_MAX);
    }
 
    assert(hmt);
    Mtree_destroy(hmt->mt);
    hashmap_destroy(hmt->hm, WHOLE);
    free(hmt);
    free(file);
    fclose(fp);
   
    return 0;
}

hm_mt *iteration(char path[], hm_mt *old) {
    //printf("path: %s\n", path);
    int mt_total_change = -1;
    int mt_internal_change = -1;
    int mt_total_nodes = -1;
    int pos_total_change = -1;
    //int pos_internal_change = -1;
    int pos_total_nodes = -1;

    int trash_a = -1;
    int trash_b = -1;
    /* prepare key value pair at first */
    int leaves_count = 0;
    char **parsed = parse(path, &leaves_count);
    key_value **leaves = parse_kvs(parsed, leaves_count);

    /* first you record the node change of pos tree */
    hashmap *hm = hashmap_create(HASHMAP_SIZE, node_destroy);
    node *root = NULL;
    /* comparison in POS tree */
    if(old) {
        create_POS(leaves, leaves_count, old->hm, &pos_total_change, &pos_total_nodes);
    } 
        
    root = create_POS(leaves, leaves_count, hm, &trash_a, &trash_b);
    /* preparing for the next iteration */

    /* next you record the node change of merkle tree */
    Mtree *current_mt = Mtree_create(leaves, leaves_count, &mt_total_nodes);

    /* comparison in merkle tree */
    if(old) {
        assert(old->mt);
        Mtree_cmp(old->mt, current_mt, &mt_total_change, &mt_internal_change);
    }
    //Mtree_write("test.txt", current_mt);

    /* write whatever needs to be written */
    if(old) {
        FILE *fp = fopen("merkleposcompare.csv", "a+");
        fprintf(fp, "%s:%s:%d:%d:%d:%d,",old->path, path, mt_total_change, pos_total_change, mt_total_nodes, pos_total_nodes); 
        fclose(fp);
    }

    /* destroy the older ones */
    if(old) {
        Mtree_destroy(old->mt);
        hashmap_destroy(old->hm, WHOLE);
        //subtree_destroy(old->root);
        free(old);
    }

    for(int i = 0; i < leaves_count; i++) {
        free(parsed[i]);
        free(leaves[i]->key);
        free(leaves[i]->value);
        free(leaves[i]);
    }
    free(parsed); 
    free(leaves);

    /* create the return value */
    hm_mt *new = malloc(sizeof(hm_mt));
    strncpy(new->path, path, 64);
    new->hm = hm;
    new->mt = current_mt;
    new->root = root;
    return new;
}

