/*
 * @file:	    hash.c
 * @brief:	    Generates a MD2 Hash of a given file
 * @date:	    19/04/2018
 * @author:	    Raphael Pour <s74020@informatik.htw-dresden.de>
 * @license:	GPL
 */

#include "hash.h"

const HASHTYPE* md2hash( const HASHTYPE *input, const HASHLENGTH length )//{{{
{
    /*
     * Variables
     */
    // Counters for iterative processing
    HASHLENGTH i,j,k,t;

    // Lengths
    HASHLENGTH paddingLength = BLOCK_LENGTH - ( length % BLOCK_LENGTH );
    HASHLENGTH lengthWithPadding = length + paddingLength;
    HASHLENGTH lengthWithPaddingAndChecksum = length + paddingLength + BLOCK_LENGTH;

    // Checksum of the input data
    HASHTYPE checksum[ BLOCK_LENGTH ];
    
    // Storage for the processed data. This storage will be increased in size
    // during the hashing process.
    HASHTYPE *currentData;

    // Storage for the output
    HASHTYPE *output;

    // Temporary variables
    HASHTYPE c;
    HASHTYPE L = 0;


    /*
     * 1) Pad message so that its length is a multiple of 16
     */
    currentData = (HASHTYPE*) malloc( sizeof(HASHTYPE) * lengthWithPadding );

    if( length > 0 )
        memcpy( currentData, input, length );

    if( currentData == NULL )
    {
        printf( "Error on reallocating Memory for padded message" );
        exit( EXIT_FAILURE );
    }

    if( input )
        for( i=0; i<paddingLength; i++ )
            currentData[ length + i ] = paddingLength;

    /*
     * 2) Calculate Checksum
     */

    // Initialize checksum with zero
    for( i=0; i<BLOCK_LENGTH; i++)
        checksum[ i ] = 0;


    // Calculate Checksum block-by-block
    for( i=0; i<lengthWithPadding/BLOCK_LENGTH; i++ )
        for( j=0; j<BLOCK_LENGTH; j++ )
        {
            c = currentData[ i * BLOCK_LENGTH + j ];
            checksum[ j ] ^= piDigits[ c ^ L ];
            L = checksum[ j ]; 
        }
    
    currentData = (HASHTYPE*) realloc ( currentData, lengthWithPaddingAndChecksum );
    if( currentData == NULL )
    {
        printf( "Error on reallocating memory for checksum\n" );
        exit( EXIT_FAILURE );
    }

    // Append checksum to padded message
    memcpy( currentData + lengthWithPadding, checksum, BLOCK_LENGTH );


    /*
     * 3) Initialize 48-Byte Digest Buffer with zero
     */
    output = (HASHTYPE*) malloc( sizeof(HASHTYPE) * 48 );
    for( i=0; i<48; i++ )
        output[ i ] = 0;

    /*
     * 4) Process message in blocks
     */
    for( i=0; i<lengthWithPaddingAndChecksum/BLOCK_LENGTH; i++ )
    {
        // Leave the first Block of the output zero
        // Fill the second Block of the output with the first 16 Bytes of the
        // current data block
        //
        // Fill the third Block of the output with the second block of the
        // output XOR first block of the output
        for( j=0; j<BLOCK_LENGTH; j++ )
        {
            output[ BLOCK_LENGTH + j ] = currentData[ i * BLOCK_LENGTH + j ];
            output[ 2 * BLOCK_LENGTH + j ] = ( output[ BLOCK_LENGTH + j ] ^ output[ j ] );
        }

        t = 0;

        for( j=0; j<ITERATION_COUNT; j++ )
        {
            for( k=0; k<48; k++ )
            {
                output[ k ] ^= piDigits[ t ];
                t = output[ k ];
            }   
            t = ( t + j ) % 256;
        }
    }   

    free( currentData );

    output = realloc( output, BLOCK_LENGTH );

    return output;
}//}}}

void dump( const HASHTYPE* buffer, const HASHLENGTH length )//{{{
{
    HASHLENGTH i;
    for( i=0; i<length; i++ )
        printf( "%02x", buffer[ i ] );
}//}}}
