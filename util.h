#pragma once

#include <stdint.h>

#define TRUE    1
#define FALSE   0

typedef char BOOL;

typedef struct files {
    char *name;
    int size;
} files_t;

typedef struct dirs {
    char *str;
    int size;
    char *cur_name;
    files_t *files;
    char *dirs_name;

    struct dirs *next;
} node_t;

int util_strcmp(const char *, const char *);

int util_memcmp(const void *, const void *, int);

void util_strcat(char *, const char *);

// Memory copy
void util_memcpy(void *, void *, int);

// Return len string
int util_strlen(const char *);

// Wipe buffer
void util_bzero(void *, size_t);

// Init linked list
node_t *util_linked_list_init(void);

// Adds a line to the end of the linked sheet
BOOL util_linked_list_add_line(node_t *, const char *, int);

// Clean linked list
void util_linked_list_clean(node_t *);

// Travers directories and Adds file paths, file size to a linked list
void util_walk_directories(node_t *, const char *);

// Called(util_linked_list_add_line)
// Return 0 if directory, 1 if file, -1 if error
int is_regular_file(const char *);