#include <stdlib.h>
#include <stdio.h>

#include "hash.h"

#ifndef BUILD_DATE
#define BUILD_DATE "fix your makefile"
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION "fix your makefile"
#endif

int main(int argc, char *argv[])
{
    printf("BuildDate: %s\n", BUILD_DATE);
    printf("BuildVersion: %s\n", BUILD_VERSION);

    int exitCode = EXIT_SUCCESS;

    printf("Test hash: ");
    const HASHTYPE *hash = md2hash( (const HASHTYPE*) "", 0);
    if ( memcmp( hash, testHash, BLOCK_LENGTH ) == 0 ) {
        puts("OK");
    }
    else
    {
        puts("NOT OK");
        exitCode = EXIT_FAILURE;
    }
    free( (void*)hash );

    return exitCode;
}


