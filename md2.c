/*
 * @file:	    md2.c
 * @brief:	    Generates a MD2 Hash of a given file
 * @date:	    19/04/2018
 * @author:	    Raphael Pour <s74020@informatik.htw-dresden.de>
 * @license:	LGPL
 */

#include "md2.h"

/*
 * Up to a data size of 128 Byte, everything is allright.
 * After this limit, the hash differes from the official hash algorithm...??
 *
 * 128 = 2^7 = 940 Bit
 */

int main(int argc, char* argv[]) // {{{
{
    FILE *inputFile = stdin;

    if(argc == 2)
    {
        if(strcmp(argv[1],"-t") == 0 || strcmp(argv[1],"--test")==0)
        {
            printf("md2(\"\"):");
            const unsigned char *hash = md2hash((const unsigned char*)"",0);

            dump(hash,16);
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

    unsigned char buffer[BUFFER_LENGTH];
    unsigned char *message = NULL;
    size_t length = 0;
    while(!feof(inputFile))
    {
        size_t read=0;
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

    dump(md2hash(message,length),16);
    puts("");

    return EXIT_SUCCESS;
} //}}}


const unsigned char* md2hash(const unsigned char *input, const size_t length)//{{{
{
    size_t paddingLength = BLOCK_LENGTH - (length % BLOCK_LENGTH);
    size_t i,j,k;
    size_t length3;
    size_t length2 = length + paddingLength;


    // 1) Pad message so that its length is a multiple of 16
    unsigned char *paddedInput;
    paddedInput = (unsigned char*)malloc(length2);

    if(length)
        memcpy(paddedInput, input,length);

    if(paddedInput == NULL)
    {
        printf("Error on reallocating Memory for padded message");
        exit(EXIT_FAILURE);
    }

    if(input)
        for(i=0;i<paddingLength;i++)
            paddedInput[length+i] = paddingLength;

    // 2) Calculate Checksum
    unsigned char checksum[BLOCK_LENGTH];
    unsigned char c;
    for(i=0;i<BLOCK_LENGTH;i++)
        checksum[i] = 0;

    unsigned char L = 0;

    for(i=0;i< length2/BLOCK_LENGTH;i++)
        for(j=0;j<BLOCK_LENGTH;j++)
        {
            c = paddedInput[i*BLOCK_LENGTH+j];
            checksum[j] ^= piDigits[c^L];
            L = checksum[j]; 
        }
    
    length3 = length + paddingLength + BLOCK_LENGTH;
    char *paddedChecksumInput  = (char*)realloc(paddedInput, length3);
    if(paddedChecksumInput == NULL)
    {
        printf("Error on reallocating memory for checksum\n");
        exit(EXIT_FAILURE);
    }

    // Copy checksum to padded message
    memcpy(paddedChecksumInput+length+paddingLength,checksum,BLOCK_LENGTH);

    // 3) Initialize 48-Byte Digest Buffer
    unsigned char X[48];
    for(i=0;i<48;i++)
        X[i] = 0;

    // 4) Process message in blocks of 16 bytes
    for(i=0;i<length3/BLOCK_LENGTH;i++)
    {
        for(j=0;j<BLOCK_LENGTH;j++)
        {
            X[BLOCK_LENGTH+j] = paddedChecksumInput[i*BLOCK_LENGTH+j];
            X[2*BLOCK_LENGTH+j] = (X[BLOCK_LENGTH+j]^X[j]);
        }

        size_t t=0;

        for(j=0;j<18;j++)
        {
            for(k=0;k<48;k++)
            {
                X[k] ^= piDigits[t];
                t = X[k];
            }   
            t = (t+j) % 256;
        }
    }   

    unsigned char *output = (unsigned char*)malloc(BLOCK_LENGTH);

    memcpy(output,(void*)X,16);
    return output;
}//}}}

void dump(unsigned const char* buffer, const size_t length)//{{{
{
    size_t i;
    for(i=0;i<length;i++)
        printf("%02x",buffer[i]);
}//}}}
