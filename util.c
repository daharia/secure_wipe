#ifdef DEBUG
    #include <stdio.h>
#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"

static files_t *node = NULL;

void util_strcat(char *src, const char *dsc) {
    util_memcpy((void*)(src + util_strlen(src)), (void *)dsc, util_strlen(dsc));
}

int util_strcmp(const char *f, const char *s) {
    int len = util_strlen(f);
    if (len != util_strlen(s))
        return FALSE;
    
    return util_memcmp(f, s, len);
}

int util_memcmp(const void *f, const void *s, int len) {
    char *f_ptr = (char *)f;
    char *s_ptr = (char *)s;
    
    while ( len-- > 0 ) {
        if (*f_ptr++ != *s_ptr++)
            return FALSE;
    }
    return TRUE;
}

void util_memcpy(void *src, void *dst, int len) {
    char *src_t = (char *)src;
    char *dst_t = (char *)dst;

    while ( len-- > 0 ) {
        *src_t++ = *dst_t++;
    }
}

int util_strlen(const char *buf) {
    int len = 0;
    char *ptr = (char *)buf;

    while (TRUE) {
        if (*ptr++ == 0) {
            break;
        }
        len++;
    }
    return len;
}

void util_bzero(void *ptr, size_t size) {
    char *pptr = (char *)ptr;

    for (int i = 0; i < size; i++) {
        *pptr++ = 0;
    }
}

BOOL util_linked_list_add_line(const char *path) {
    files_t *new = NULL;
    int len;
    
    new = (files_t *)malloc(sizeof(files_t));
    if (new == NULL) {
        #ifdef DEBUG
        printf("[util_linked_list_add_line] malloc is NULL\n");
        #endif
        return FALSE;
    }
    len = util_strlen(path);

    new->path = (char *)malloc(sizeof(char) * len + 1);
    if (new->path == NULL) {
        #ifdef DEBUG
        printf("[util_linked_list_add_line] malloc is NULL\n");
        #endif
        free(new);
        return FALSE;
    }
    util_memcpy((void*)new->path, (void*)path, len);
    new->path[len + 1] = 0; // End of string is '\0'
    new->next = NULL;

    if (node) { // if node != NULL, append node to next
        files_t *ptr = node;
        while(ptr->next)
            ptr = ptr->next;
        ptr->next = new;
    } else {
        node = new;
    }
    return TRUE;  
}

void util_linked_list_clean(void) {
    files_t *ptr = node;
    while (ptr) {
        if (ptr->path != NULL) {
            free(ptr->path);
        }
        files_t *previous = ptr;
        ptr = ptr->next;
        free(previous);
    }
}

#ifdef DEBUG
void util_linked_list_print_all_path(void) {
    files_t *ptr = node;
    int number = 0;

    while (ptr) {
        printf("%s\n", ptr->path);
        number += 1;
        ptr = ptr->next;
    }
    printf("Number of files: %d\n", number);
}
#endif

void util_walk_directories(const char *path) {
    DIR *d;
    struct dirent *dir;
    char buf[_MAX_PATH];
    int err;

    // chdir("c:\\");

    err = util_is_regular_file(path);
    if (err == -1) {
        #ifdef DEBUG
        printf("[util_walk_directories] util_is_regular_file(%s) == %d\n", path, err);
        #endif
        return;
    }
    #ifdef DEBUG
    else if (err == -2) {
        printf("[util_walk_directories] util_is_regular_file(%s) == %d\n", path, err);
        return;
    }
    #endif
    else if (err >= 0) {
        util_linked_list_add_line(path);
        return;
    }
    
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (util_strcmp(dir->d_name, "."))
                continue;
            else if (util_strcmp(dir->d_name, ".."))
                continue;

            util_bzero(buf, _MAX_PATH);
            util_strcat(buf, path);
            util_strcat(buf, "\\");
            util_strcat(buf, dir->d_name);
            
            err = util_is_regular_file(buf);
            if (err == -1) {
                #ifdef DEBUG
                printf("[util_walk_directories] util_is_regular_file(%s) == %d\n", buf, err);
                #endif
                continue;
            }
            else if (err == -2)
                util_walk_directories(buf);
            else
                util_linked_list_add_line(buf);
        }
        closedir(d);
    }
}

int util_is_regular_file(const char *path) {
    struct stat path_stat;
    
    if (stat(path, &path_stat) != 0) {
        return -1;
    }
    if (S_ISREG(path_stat.st_mode))
        return path_stat.st_size;
    else
        return -2;
}

void util_wipe_files(void) {
    files_t *ptr = node;

    while (ptr) {
        util_wipe_file(ptr->path);
        ptr = ptr->next;
    }
}

BOOL util_wipe_file(const char *path) {
    int cycle_count = 2; // Number of rewrites

    int fd, err, size;
    int quantity, remainder;
    char buffer[BUFFER_MAX];

    size = util_is_regular_file(path);
    if (size <= 0) {
        #ifdef DEBUG
        printf("[wipe] util_is_regular_file('%s') == %d\n", path, size);
        #endif
        return FALSE;
    }
    util_bzero((void *)buffer, BUFFER_MAX);
    quantity = size / BUFFER_MAX;
    remainder = size % BUFFER_MAX;

    fd = open(path, O_WRONLY);
    if (fd == -1) {
        #ifdef DEBUG
        printf("[wipe] open('%s') == %d\n", path, fd);
        #endif
        return FALSE;
    }
    for (int cycle = 0; cycle < cycle_count; cycle++) {

        for (int i = 0; i < quantity; i++) {
            err = write(fd, buffer, BUFFER_MAX);
            if (err == -1) {
                #ifdef DEBUG
                printf("[wipe] write('%s') == %d\n", path, err);
                #endif
                goto err_break;
            }
        }
        err = write(fd, buffer, remainder);
        if (err == -1) {
            #ifdef DEBUG
            printf("[wipe] write('%s') == %d\n", path, err);
            #endif
            goto err_break;
        }
        err = lseek(fd, 0, SEEK_SET);
        if (err == -1) {
            #ifdef DEBUG
            printf("[wipe] write('%s') == %d\n", path, err);
            #endif
            goto err_break;
        }
    }

    err_break:
    close(fd);

    #ifndef DEBUG
    remove(path);
    #endif

    return TRUE;
}
