/*
 * @file:	    md2.c
 * @brief:	    Generates a MD2 Hash of a given file
 * @date:	    19/04/2018
 * @author:	    Raphael Pour <s74020@informatik.htw-dresden.de>
 * @license:	LGPL
 */

#include "md2.h"

int main(int argc, char* argv[]) // {{{
{
    FILE *inputFile = stdin;

    if(argc == 2)
    {
        if(strcmp(argv[1],"-t") == 0 || strcmp(argv[1],"--test")==0)
        {
            printf("md2(\"\"):");
            const HASHTYPE *hash = md2hash((const HASHTYPE*)"",0);

            dump(hash,BLOCK_LENGTH);
            printf(" %s\n", (memcmp(hash,testHash, BLOCK_LENGTH)==0) ? "OK" : "NOT OK");

            return EXIT_SUCCESS;
        }
        else if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help")==0)
        {
            printf("usage: %s [<filename>|-t|--test|-h|--help]\n", argv[0]);
            puts("If no argument provided, md2 will read from stdin until EOF");
            puts("t ... Tests hashfunction by computing md2(\"\")");
            puts("h ... Prints help");

            return EXIT_SUCCESS;
        }
        else
        {
            inputFile = fopen(argv[1],"rb");
            if(inputFile == NULL)
            {
                perror("Error opening input file");
                exit(EXIT_FAILURE);
            }
        }
    }

    HASHTYPE buffer[BUFFER_LENGTH];
    HASHTYPE *message = NULL;
    HASHLENGTH length = 0;
    while(!feof(inputFile))
    {
        HASHLENGTH read=0;
        read = fread(&buffer,1, BUFFER_LENGTH, inputFile);

        if(read == 0)
        {
            if(feof(inputFile))
                break;
            printf("Error on reading file: %i\n", ferror(inputFile));
            exit(EXIT_FAILURE);
        }

        message = realloc(message, length+read);
        if(message == NULL)
        {
            perror("Error on allocating memory for input file");
            exit(EXIT_FAILURE);
        }

        memcpy((message+length),&buffer,read);
        length += read;
    }
    fclose(inputFile);

    dump(md2hash(message,length),BLOCK_LENGTH);
    puts("");

    return EXIT_SUCCESS;
} //}}}

const HASHTYPE* md2hash(const HASHTYPE *input, const HASHLENGTH length)//{{{
{
    /*
     * Variables
     */
    // Counters for iterative processing
    HASHLENGTH i,j,k,t;

    // Lengths
    HASHLENGTH paddingLength = BLOCK_LENGTH - (length % BLOCK_LENGTH);
    HASHLENGTH lengthWithPadding = length + paddingLength;
    HASHLENGTH lengthWithPaddingAndChecksum;

    // Checksum of the input data
    HASHTYPE checksum[BLOCK_LENGTH];
    
    // Storage for the processed data. This storage will be increased in size
    // during the hashing process.
    HASHTYPE *currentData;

    // Storage for the output
    HASHTYPE *output;

    // Temporary variables
    HASHTYPE c;
    HASHTYPE L = 0;

    // 1) Pad message so that its length is a multiple of 16
    currentData = (HASHTYPE*) malloc( sizeof(HASHTYPE) * lengthWithPadding);

    if(length)
        memcpy(currentData, input,length);

    if(currentData == NULL)
    {
        printf("Error on reallocating Memory for padded message");
        exit(EXIT_FAILURE);
    }

    if(input)
        for( i=0; i<paddingLength; i++ )
            currentData[ length + i ] = paddingLength;

    // 2) Calculate Checksum
    for( i=0; i<BLOCK_LENGTH; i++)
        checksum[i] = 0;


    for( i=0; i<lengthWithPadding/BLOCK_LENGTH; i++ )
        for( j=0; j<BLOCK_LENGTH; j++ )
        {
            c = currentData[ i * BLOCK_LENGTH + j];
            checksum[j] ^= piDigits[c^L];
            L = checksum[j]; 
        }
    
    lengthWithPaddingAndChecksum = lengthWithPadding + BLOCK_LENGTH;
    currentData = (HASHTYPE*) realloc (currentData, lengthWithPaddingAndChecksum);

    if(currentData == NULL)
    {
        printf("Error on reallocating memory for checksum\n");
        exit(EXIT_FAILURE);
    }

    // Copy checksum to padded message
    memcpy(currentData + lengthWithPadding, checksum, BLOCK_LENGTH);

    // 3) Initialize 48-Byte Digest Buffer
    output = (HASHTYPE*) malloc(sizeof(HASHTYPE) * 48);
    for( i=0; i<48; i++ )
        output[i] = 0;

    // 4) Process message in blocks of 16 bytes
    for( i=0; i<lengthWithPaddingAndChecksum/BLOCK_LENGTH; i++ )
    {
        for( j=0; j<BLOCK_LENGTH; j++ )
        {
            output[ BLOCK_LENGTH + j ] = currentData[ i * BLOCK_LENGTH + j ];
            output[ 2 * BLOCK_LENGTH + j ] = ( output[BLOCK_LENGTH + j] ^ output[j] );
        }

        t=0;

        for( j=0; j<ITERATION_COUNT; j++ )
        {
            for( k=0; k<48; k++ )
            {
                output[k] ^= piDigits[t];
                t = output[k];
            }   
            t = ( t+j ) % 256;
        }
    }   

    free(currentData);

    memset(output + 16, 0, 32);

    return output;
}//}}}

void dump(const HASHTYPE* buffer, const HASHLENGTH length)//{{{
{
    HASHLENGTH i;
    for(i=0;i<length;i++)
        printf("%02x",buffer[i]);
}//}}}
