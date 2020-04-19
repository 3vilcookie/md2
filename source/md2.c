/*
 * @file:	    md2.c
 * @brief:	    Generates a MD2 Hash of a given file
 * @date:	    19/04/2018
 * @author:	    Raphael Pour <s74020@informatik.htw-dresden.de>
 * @license:	GPL
 */

#include "hash.h"

int main( int argc, char* argv[] ) // {{{
{
    FILE *inputFile = stdin;

    HASHTYPE buffer[ BUFFER_LENGTH ];
    HASHTYPE *message = NULL;
    HASHLENGTH length = 0;
    HASHLENGTH read=0;

    // This programm is designed as a filter except a switch is provided.
    // No arguments means: read from stdin.
    // Filename as argument means: read from this file.
    if( argc == 2 )
    {
        // Tests hash algorithm by hashing "" and compare it with a hardcoded
        // one
        if( strcmp(argv[1],"-t") == 0 || strcmp(argv[1],"--test") ==0 )
        {
            printf( "md2(\"\"):" );
            const HASHTYPE *hash = md2hash( (const HASHTYPE*) "", 0);

            dump( hash, BLOCK_LENGTH );
            printf( " %s\n", ( memcmp( hash, testHash, BLOCK_LENGTH ) == 0 ) ? "OK" : "NOT OK" );

            free( (void*) hash );
            return EXIT_SUCCESS;
        }
        // Show help
        else if( strcmp(argv[1],"-h") == 0 || strcmp( argv[1],"--help") == 0 )
        {
            printf( "usage: %s [<filename>|-t|--test|-h|--help]\n", argv[0] );
            puts( "If no argument provided, md2 will read from stdin until EOF" );
            puts( "t ... Tests hashfunction by computing md2(\"\")" );
            puts( "h ... Prints help" );

            return EXIT_SUCCESS;
        }
        // Interpret argument as file and open it
        else
        {
            inputFile = fopen( argv[1], "rb" );
            if( inputFile == NULL )
            {
                perror( "Error opening input file" );
                return EXIT_FAILURE ;
            }
        }
    }

    /*
     * Copy Data from stream -file or stdin- into a buffer until EOF is reached
     */
    while( ! feof( inputFile ) )
    {
        read = fread( &buffer, 1, BUFFER_LENGTH, inputFile );

        if( read == 0 )
        {
            if( feof( inputFile ) )
                break;
            printf( "Error on reading file: %i\n", ferror( inputFile ) );
            return EXIT_FAILURE ;
        }

        message = realloc( message, length + read );
        if( message == NULL )
        {
            perror( "Error on allocating memory for input file" );
            return EXIT_FAILURE ;
        }

        memcpy( message + length, &buffer, read);
        length += read;
    }

    fclose( inputFile );

    // Generate hash and write it to stdout ('cause that's what filters do)
    dump( md2hash( message, length ), BLOCK_LENGTH );
    puts( "" );
    
    free( message );

    return EXIT_SUCCESS;
} //}}}
