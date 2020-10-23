
/* I have referred to https://stackoverflow.com/questions/1271064/how-do-i-loop-through-all-./files-in-a-folder-using-c  to make this code below */
#include "main.h"
int verbosity = POS_VERBOSE;

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
    verbosity = POS_DEBUG;

    hm_mt *hmt = NULL;

    struct dirent *dp;
    DIR *dfd;
    char *dir;
    if(argc == 1) {
        fprintf(stderr, "Usage: %s dirname\n", argv[0]);
        return EXIT_FAILURE;
    }
    dir = argv[1];
    char filename_qfd[PATH_MAX];

    if((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open %s\n", dir);
        return EXIT_FAILURE;
    }

    while((dp = readdir(dfd)) != NULL) {
        struct stat stbuf;

        if(dir[strlen(dir) - 1] == '/') {
            sprintf(filename_qfd, "%s%s", dir, dp->d_name);
        } else {
            sprintf(filename_qfd, "%s/%s", dir, dp->d_name);
        }

        if(stat(filename_qfd, &stbuf) == -1) {
            fprintf(stderr, "Unable to stat file: %s\n", filename_qfd);
            continue;
        }
        if((stbuf.st_mode & S_IFMT) == S_IFDIR) {
            fprintf(stderr, "%s is a directory... skipping\n", filename_qfd);
            continue;
        } else {
            DEBUG("filename: %s\n", filename_qfd);
            hmt = iteration(filename_qfd, hmt);
        }
    }
    /*
    for(int i = 0; i < 9; i++) {
        hmt = iteration(paths[i], hmt);    
    }
    */

    assert(hmt);
    Mtree_destroy(hmt->mt);
    hashmap_destroy(hmt->hm, WHOLE);
    free(hmt);
    
    return 0;
}

hm_mt *iteration(char path[], hm_mt *old) {
    printf("path: %s\n", path);
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

