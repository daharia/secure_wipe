#include <stdlib.h>
#include "util.h"

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    rand_init();
    
    for (int i = 1; i < argc; i++)
        util_walk_directories(argv[i]);

    util_wipe_files();
}