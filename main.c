#include <stdio.h>

#include <dirent.h>
#include "util.h"

int main(int argc, char **argv) {
    node_t *tail;

    chdir("c:\\");

    if (argc < 2) {
        printf("error!\n");
        return 1;
    }
    
    // tail = util_linked_list_init();
    // if (tail == NULL) {
    //     return 1;
    // }
    util_walk_directories(NULL, argv[1]);
    
//     char* buffer;
//    // Get the current working directory:
//    if ( (buffer = _getcwd( NULL, 0 )) == NULL )
//       perror( "_getcwd error" );
//    else
//    {
//       printf( "%s \nLength: %zu\n", buffer, strlen(buffer) );
//       free(buffer);
//    }
}