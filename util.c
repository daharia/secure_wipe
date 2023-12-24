#ifdef DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"
#include "rand.h"

static files_t *linked_list = NULL;

void util_strcat(char *src, const char *dsc)
{
    util_memcpy((void *)(src + util_strlen(src)), (void *)dsc, util_strlen(dsc));
}

int util_strcmp(const char *f, const char *s)
{
    int len = util_strlen(f);
    if (len != util_strlen(s))
        return FALSE;

    return util_memcmp(f, s, len);
}

int util_memcmp(const void *f, const void *s, int len)
{
    char *f_ptr = (char *)f;
    char *s_ptr = (char *)s;

    while (len-- > 0)
    {
        if (*f_ptr++ != *s_ptr++)
            return FALSE;
    }
    return TRUE;
}

void util_memcpy(void *src, void *dst, int len)
{
    char *src_t = (char *)src;
    char *dst_t = (char *)dst;

    while (len-- > 0)
    {
        *src_t++ = *dst_t++;
    }
}

int util_strlen(const char *buf)
{
    int len = 0;
    char *ptr = (char *)buf;

    while (TRUE)
    {
        if (*ptr++ == 0)
        {
            break;
        }
        len++;
    }
    return len;
}

void util_bzero(void *ptr, size_t size)
{
    char *pptr = (char *)ptr;

    for (int i = 0; i < size; i++)
    {
        *pptr++ = 0;
    }
}

BOOL util_linked_list_add_line(const char *path, char type)
{
    files_t *new = NULL;
    int len;

    new = (files_t *)malloc(sizeof(files_t));
    if (new == NULL)
    {
#ifdef DEBUG
        printf("[util_linked_list_add_line] malloc is NULL\n");
#endif
        return FALSE;
    }
    len = util_strlen(path) + 1;

    new->path = (char *)malloc(sizeof(char) * len);
    if (new->path == NULL)
    {
#ifdef DEBUG
        printf("[util_linked_list_add_line] malloc is NULL\n");
#endif
        free(new);
        return FALSE;
    }
    util_memcpy((void *)new->path, (void *)path, len);
    new->next = NULL;
    new->type = type;

    new->next = linked_list;
    linked_list = new;

    return TRUE;
}

void util_linked_list_clean(void)
{
    files_t *ptr = linked_list;
    while (ptr)
    {
        if (ptr->path != NULL)
            free(ptr->path);

        files_t *previous = ptr;
        ptr = ptr->next;
        free(previous);
    }
}

void util_walk_directories(const char *path)
{
    DIR *d;
    struct dirent *dir;
    char buf[_MAX_PATH];
    int err;

    // chdir("c:\\");

    err = util_is_regular_file(path);
    if (err == -1)
    {
#ifdef DEBUG
        printf("[util_walk_directories] util_is_regular_file(%s) == %d\n", path, err);
#endif
        return;
    }
    else if (err >= 0)
    {
        util_linked_list_add_line(path, FILE);
        return;
    }

    d = opendir(path);
    if (d)
    {
        util_linked_list_add_line(path, DIRECTORY);

        while ((dir = readdir(d)) != NULL)
        {
            if (util_strcmp(dir->d_name, "."))
                continue;
            else if (util_strcmp(dir->d_name, ".."))
                continue;

            util_bzero(buf, _MAX_PATH);
            util_strcat(buf, path);
            util_strcat(buf, "\\");
            util_strcat(buf, dir->d_name);

            err = util_is_regular_file(buf);
            if (err == -1)
            {
#ifdef DEBUG
                printf("[util_walk_directories] util_is_regular_file(%s) == %d\n", buf, err);
#endif
                continue;
            }
            else if (err == -2)
                util_walk_directories(buf);
            else
                util_linked_list_add_line(buf, FILE);
        }
        closedir(d);
    }
}

int util_is_regular_file(const char *path)
{
    struct stat path_stat;

    if (stat(path, &path_stat) != 0)
    {
        return -1;
    }
    if (S_ISREG(path_stat.st_mode))
        return path_stat.st_size;
    else
        return -2;
}

void util_wipe_files(void)
{
    files_t *ptr = linked_list;

    while (ptr) // Wipe files
    {
        if (ptr->type == FILE)
            util_wipe_file(ptr);

        ptr = ptr->next;
    }

    ptr = linked_list;

    while (ptr) // Wipe directory
    {
        if (ptr->type == DIRECTORY)
            util_wipe_file(ptr);

        ptr = ptr->next;
    }

    util_linked_list_clean();
}

void util_wipe_file(files_t *ptr)
{
    int cycle_count = 2; // Number of rewrites

    int fd, err, size;
    int quantity, remainder;
    char buffer[BUFFER_MAX];

    if (ptr->type == DIRECTORY)
        goto rename_and_delete;

    size = util_is_regular_file(ptr->path); // check size again if file!
    if (size == 0)
        size = 100;

    util_bzero((void *)buffer, BUFFER_MAX);
    quantity = size / BUFFER_MAX;
    remainder = size % BUFFER_MAX;

    fd = open(ptr->path, O_WRONLY);
    if (fd == -1)
    {
#ifdef DEBUG
        printf("[wipe] open('%s') == %d\n", ptr->path, fd);
#endif
        return;
    }
    for (int cycle = 0; cycle < cycle_count; cycle++)
    {
        for (int i = 0; i < quantity; i++)
        {
            err = write(fd, buffer, BUFFER_MAX);
            if (err == -1)
            {
#ifdef DEBUG
                printf("[wipe] write('%s') == %d\n", ptr->path, err);
#endif
                goto err_break;
            }
        }
        err = write(fd, buffer, remainder);
        if (err == -1)
        {
#ifdef DEBUG
            printf("[wipe] write('%s') == %d\n", ptr->path, err);
#endif
            goto err_break;
        }
        err = lseek(fd, 0, SEEK_SET);
        if (err == -1)
        {
#ifdef DEBUG
            printf("[wipe] write('%s') == %d\n", ptr->path, err);
#endif
            goto err_break;
        }
    }

err_break:
    close(fd);

rename_and_delete:
    char buf[_MAX_PATH];

    int len_name = util_name_len(ptr->path);
    int len_path = util_strlen(ptr->path);

    util_bzero(buf, _MAX_PATH);
    util_strcat(buf, ptr->path);

    rand_string(buf + (len_path - len_name), len_name);

    rename(ptr->path, buf);

    if (ptr->type == FILE)
        unlink(buf);
    else
        rmdir(buf);
}

int util_name_len(const char *path)
{
    int i, full;

    full = i = util_strlen(path);
    if (i <= 0)
        return 5;

    while (i > 0 && (path[i] != '\\' && path[i] != '/'))
        i--;

    return full - i - 1;
}
