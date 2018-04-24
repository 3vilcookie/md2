/*
 * @file:	md2.c
 * @brief:	Generates a MD5 Hash of a given file
 * @date:	19/04/2018
 * @author:	Raphael Pour <s74020@informatik.htw-dresden.de>
 * @license:	LGPL
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char piDigits[256] = {
  41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
  19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
  76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
  138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
  245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
  148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
  39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
  181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
  150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
  112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
  96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
  85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
  234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
  129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
  8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
  203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
  166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
  31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

const unsigned short BLOCK_LENGTH = 16;

const unsigned char* md2hash(const char *input, size_t length);
void dump(const unsigned char* buffer, const size_t length);

int main(int argc, char* argv[])
{

    dump(md2hash("Raphael Pour",12),16);

    return EXIT_SUCCESS;
}

const unsigned char* md2hash(const char *input, size_t length)
{
    size_t paddingLength = BLOCK_LENGTH - (length % BLOCK_LENGTH);
    size_t i,j,k;
    size_t length3;
    size_t length2 = length + paddingLength;

    
    printf("Raw input -----------\n");
    dump((const unsigned char*)input,length);

    // 1) Pad message so that its length is a multiple of 16
    unsigned char *paddedInput;
    paddedInput = (unsigned char*)malloc(length2);
        
    if(length)
        memcpy(paddedInput, input,length);

    printf("Unpadded input ------\n");
    dump(paddedInput, length2);

    if(paddedInput == NULL)
    {
        printf("Error on reallocating Memory for padded message");
        exit(EXIT_FAILURE);
    }
 
    if(input)
    for(i=0;i<paddingLength;i++)
        paddedInput[length+i] = paddingLength;
    
    printf("padded input --------\n");
    dump(paddedInput,length2);

    // 2) Calculate Checksum
    unsigned char checksum[BLOCK_LENGTH];
    char c;
    for(i=0;i<BLOCK_LENGTH;i++)
        checksum[i] = 0;
    
    printf("Empty Checksum --------\n");
    dump(checksum,BLOCK_LENGTH);

    unsigned char L = 0;

    for(i=0;i< length2/BLOCK_LENGTH;i++)
        for(j=0;j<BLOCK_LENGTH;j++)
        {
          c = paddedInput[i*16+j];
          checksum[j] ^= piDigits[c^L];
          L = checksum[j]; 
        }
    printf("Checksum ------------\n");
    dump(checksum,16);


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
    
    dump(X,48);

    unsigned char *output = (unsigned char*)malloc(16);

    memcpy(output,(void*)X,16);
    return output;
}

void dump(unsigned const char* buffer, const size_t length)
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
}
