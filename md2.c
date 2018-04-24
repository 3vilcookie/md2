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
    FILE *inputFile;

    if(argc < 2)
        inputFile = stdin;
    else 
    {
        inputFile = fopen(argv[1],"rb");
        if(inputFile == NULL)
        {
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }
    }

    char buffer[BUFFER_LENGTH];
    char *message = NULL;
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

    printf("Read: %lu\n",length);
    dump(md2hash(message,length),16);

    return EXIT_SUCCESS;
} //}}}

const unsigned char* md2hash(const char *input, size_t length)//{{{
{
    size_t paddingLength = BLOCK_LENGTH - (length % BLOCK_LENGTH);
    size_t i,j,k;
    size_t length3;
    size_t length2 = length + paddingLength;

    
    
    //printf("Raw input -----------\n");
    //dump((const unsigned char*)input,length);

    // 1) Pad message so that its length is a multiple of 16
    unsigned char *paddedInput;
    paddedInput = (unsigned char*)malloc(length2);
        
    if(length)
        memcpy(paddedInput, input,length);

    //printf("Unpadded input ------\n");
    //dump(paddedInput, length2);

    if(paddedInput == NULL)
    {
        printf("Error on reallocating Memory for padded message");
        exit(EXIT_FAILURE);
    }
 
    if(input)
    for(i=0;i<paddingLength;i++)
        paddedInput[length+i] = paddingLength;
    
    //printf("padded input --------\n");
    //dump(paddedInput,length2);

    // 2) Calculate Checksum
    unsigned char checksum[BLOCK_LENGTH];
    char c;
    for(i=0;i<BLOCK_LENGTH;i++)
        checksum[i] = 0;
    
    //printf("Empty Checksum --------\n");
    //dump(checksum,BLOCK_LENGTH);

    unsigned char L = 0;

    for(i=0;i< length2/BLOCK_LENGTH;i++)
        for(j=0;j<BLOCK_LENGTH;j++)
        {
          c = paddedInput[i*16+j];
          checksum[j] ^= piDigits[c^L];
          L = checksum[j]; 
        }
    //printf("Checksum ------------\n");
    //dump(checksum,16);


    length3 = length + paddingLength + 16;
    char *paddedChecksumInput  = (char*)realloc(paddedInput, length3);
    if(paddedChecksumInput == NULL)
    {
        printf("Error on reallocating memory for checksum\n");
        exit(EXIT_FAILURE);
    }

    // Copy checksum to padded message
    memcpy(paddedChecksumInput+length+paddingLength,checksum,16);

    // 3) Initialize 48-Byte Digest Buffer
    unsigned char X[48];
    for(i=0;i<48;i++)
        X[i] = 0;

    // 4) Process message in blocks of 16 bytes
    for(i=0;i<length3/16;i++)
    {
        for(j=0;j<16;j++)
        {
            X[16+j] = paddedChecksumInput[i*16+j];
            X[32+j] = (X[16+j]^X[j]);
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
    
    //dump(X,48);

    unsigned char *output = (unsigned char*)malloc(16);

    memcpy(output,(void*)X,16);
    return output;
}//}}}

void dump(unsigned const char* buffer, const size_t length)//{{{
{
    size_t i;
    for(i=0;i<length;i++)
    {
        printf("%02x",buffer[i]);
    
        if(i %16 == 15)
            printf("\n");
        else if(i % 8 == 7)
            printf(" ");
    }
    puts("");
}//}}}
