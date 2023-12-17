#include <stdlib.h>
#include "util.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    

    for (int i = 1;i < argc; i++)
        util_walk_directories(argv[i]);
    
    #ifdef DEBUG
    util_linked_list_print_all_path();
    #endif

    util_wipe_files();

    util_linked_list_clean();
}