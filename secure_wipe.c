
// #ifdef DEBUG
#include <stdio.h>
// #endif

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <direct.h>

#include "util.h"
#include "secure_wipe.h"

BOOL wipe(const char *path, int size) {
    int cycle_count = 3; // Default 3

    int fd;
    int err;
    int quantity, remainder;

    char buffer[BUFFER_MAX];

    util_bzero((void *)buffer, BUFFER_MAX);
    quantity = size / BUFFER_MAX;
    remainder = size % BUFFER_MAX;

    fd = open(path, O_WRONLY);
    if (fd == -1) {
        return FALSE;
    }
    for (int cycle = 0; cycle < cycle_count; cycle++) {

        for (int i = 0; i < quantity; i++) {
            err = write(fd, buffer, BUFFER_MAX);
            if (err == -1) {
                goto err_break;
            }
        }
        err = write(fd, buffer, remainder);
        if (err == -1) {
            goto err_break;
        }
        err = lseek(fd, 0, SEEK_SET);
        if (err == -1) {
            goto err_break;
        }
    }

    err_break:
    close(fd);

    return TRUE;
}

