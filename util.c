#ifdef DEBUG
    #include <stdio.h>
#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

#include "util.h"

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

node_t *util_linked_list_init(void) {
    node_t *head = NULL;
    
    return (node_t *)malloc(sizeof(node_t));   
}

BOOL util_linked_list_add_line(node_t *node, const char *str, int size) {
    node_t *ptr = node;

    while (ptr != NULL) {
        if (ptr->str != NULL) {
            ptr = ptr->next;
        }
        else {
            int len = util_strlen(str);
            
            ptr->str = (char *)malloc(len + 1);
            if (ptr->str == NULL) {
                return FALSE;
            }
            ptr->size = size;
            util_memcpy((void*)ptr->str, (void*)str, len);
            
            return TRUE;
        }
    }
    return FALSE;
}

void util_linked_list_clean(node_t *node) {
    node_t *ptr = node;
    node_t *tmp;

    while (ptr->next != NULL) {
        if (ptr->str != NULL) {
            free(ptr->str);
        }
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
}

void util_walk_directories(node_t *node, const char *path) {
    DIR *d;
    struct dirent *dir;
    char buf[_MAX_PATH];

    if (is_regular_file(path)) {
        printf("%s\n", buf);
        printf("%s\n", buf);
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
            
            if (is_regular_file(buf)) {
                printf("%s\n", buf);
            } else {
                util_walk_directories(node, buf);
            }
        }
        closedir(d);
    }
    #ifdef DEBUG
    else {
        printf("[util_walk_directories] opendir error\n");
    }
    #endif
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    if (S_ISREG(path_stat.st_mode)) {
        // util_linked_list_add_line(node, path, path_stat.st_size);
        return 1;
    }
    return 0;
}
