#include <stdlib.h>

// #if defined(_WIN32) || defined (_WIN64) 
//     #include <windows.h>
//     #define WINDOWS 1
// #endif


// static int fd_urandom = -1;


void rand_init()
{
// #ifdef WINDOWS 
    
// #else
//     fd_urandom = open( "/dev/urandom", "rb");
//     if( !f) ...
//         fread( binary_string, string_length, f);
//     fclose(f);
// #endif

    srand(time(0));
}

int rand_get()
{
    return rand();
}


void rand_string(char *buf, int size)
{
    const char *ascii_num = "abcdefghijklmnopqrstuvwxyz0123456789";
    const int ascii_num_len = util_strlen(ascii_num);
    int key;

    for (int n = 0; n < size; n++)
    {
        key = rand_get() % ascii_num_len;
        buf[n] = ascii_num[key];
    }
}
