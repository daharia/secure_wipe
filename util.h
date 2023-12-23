#pragma once

#include <stdint.h>

#define BUFFER_MAX 1000

#define TRUE 1
#define FALSE 0

typedef char BOOL;

typedef struct files
{
    char *path;
    struct files *next;
    struct files *prev;
} files_t;

// Compare lines
int util_strcmp(const char *, const char *);

// Compare memory
int util_memcmp(const void *, const void *, int);

// Copy a line to the end of another line
void util_strcat(char *, const char *);

// Memory copy
void util_memcpy(void *, void *, int);

// Return len string
int util_strlen(const char *);

// Wipe buffer
void util_bzero(void *, size_t);

// Adds a line to the end of the linked sheet
BOOL util_linked_list_add_line(const char *);

// Wipe files and Clean linked list
void util_linked_list_clean(void);

// Travers directories and Adds file paths, file size to a linked list
void util_walk_directories(const char *);

// Return -1 if error, -2 if directory, size of file if path is file
int util_is_regular_file(const char *);

// ...
void util_wipe_files(void);

// Return TRUE if OK, else FALSE
BOOL util_wipe_file(const char *);

int util_name_len(const char *);
