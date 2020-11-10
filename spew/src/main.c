#include "main.h"

/* DO NOT USE THE FOLLOWING FUNCTION WHEN YOU ARE READING, IT CHANGES THE FILE POINTER TO THE BEGINNING OF THE FILE!!! */
size_t get_size(FILE *fp) {
    fseek(fp, 0L, SEEK_END);
    size_t sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return sz;
}

size_t recursive_write(char *dirname, FILE *fp_w, size_t offset, list_t *metalist) {
    struct dirent *dp;
    DIR *dfd;
    char *dir;
    dir = dirname;
    char page [4096];
    char *filename_qfd = malloc(PATH_MAX);

    if((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "Cannot open %s\n", dir);
        return -1;
    }
    while((dp = readdir(dfd)) != NULL) {
        struct stat stbuf;
        size_t len = strlen(dp->d_name);
        if(!strncmp(dp->d_name, ".", len) || !strncmp(dp->d_name, "..", len) ) {
            continue;
        }
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
            //fprintf(stderr, "directory: %s\n", filename_qfd); 
            offset = recursive_write(filename_qfd, fp_w, offset, metalist);
        } else if((stbuf.st_mode & S_IFMT) == S_IFCHR) {
            //fprintf(stderr, "this is character-oriented device file: %s\n", filename_qfd);
        } else if((stbuf.st_mode & S_IFMT) == S_IFBLK) {
            //fprintf(stderr, "this is block-oriented device file: %s\n", filename_qfd);
        } else if((stbuf.st_mode & S_IFMT) == S_IFREG) {
            fprintf(stderr, "filename: %s\n", filename_qfd);
            FILE *fp_r = fopen(filename_qfd, "r");
            if(!fp_r) {
                fprintf(stderr, "could not open %s, exitting...\n", filename_qfd);
                perror("");
                exit(1);
            }
            size_t sz = get_size(fp_r);
            if(add_metadata(filename_qfd, sz, offset, metalist)) {
                fprintf(stderr, "adding metadata for %s failed, exitting...\n", filename_qfd);
                exit(1);
            }
            
            size_t read;
            size_t wrote;
            while((read = fread(page, sizeof(char), 4096, fp_r)) > 0) {
                wrote = fwrite(page, sizeof(char), read, fp_w);
                assert(read == wrote);
            }
            offset += sz;
            fclose(fp_r);
        } else if((stbuf.st_mode & S_IFMT) == S_IFLNK) {
            //fprintf(stderr, "this is symbolic link: %s\n", filename_qfd);
            /*
            FILE *fp_r = fopen(filename_qfd, "r");
            if(!fp_r) {
                fprintf(stderr, "symlink didnt open: %s\n", filename_qfd);
                continue;
            }
            size_t sz = get_size(fp_r);
            if(add_metadata(filename_qfd, sz, offset, metalist)) {
                fprintf(stderr, "adding metadata for %s failed, exitting...\n", filename_qfd);
                exit(1);
            }
            size_t read;
            size_t wrote;
            while((read = fread(page, sizeof(char), 4096, fp_r)) > 0) {
                wrote = fwrite(page, sizeof(char), read, fp_w);
                assert(read == wrote);
            }
            offset += sz;
            fclose(fp_r);
            */

        } else if((stbuf.st_mode & S_IFMT) == S_IFSOCK) {
            //fprintf(stderr, "this is socket: %s\n", filename_qfd);
        } else if((stbuf.st_mode & S_IFMT) == S_IFIFO) {
            //fprintf(stderr, "this is FIFO or pipe: %s\n", filename_qfd);
        } else {
            //fprintf(stderr, "should not be here exitting...\n");
            exit(1);
        }
    }

    closedir(dfd);
    free(filename_qfd);
    return offset;
}

int main(int argc, char **argv) {
    char *dir;
    list_t *metalist = init_metadata();
    size_t offset = 0;
    char datafile [PATH_MAX];
    char metafile [PATH_MAX];

    dir = argv[1];
    size_t dirlen = strlen(dir) - 1;
    if(dir[dirlen] == '/') {
        dir[dirlen] = '\0';
    }
        
    strncpy(datafile, dir, PATH_MAX);
    strncat(datafile, ".data", PATH_MAX);
    strncpy(metafile, dir, PATH_MAX);
    strncat(metafile, ".meta", PATH_MAX);
    
    FILE *fp_w = fopen(datafile, "w");
    
    if(argc != 2) {
       fprintf(stderr, "Usage: %s dirname\n", argv[0]);
       return EXIT_FAILURE; 
    }

    offset = recursive_write(dir, fp_w, offset, metalist);

    fclose(fp_w);
    if(write_metadata(metalist, metafile)) {
        fprintf(stderr, "write metadata failed\n");
        return EXIT_FAILURE;
    }
    file_t *node;
    while(metalist->size > 0) {
        node = pop_metadata(metalist);
        free(node);
    }
    destroy_metadata(metalist);
    return EXIT_SUCCESS;
}



