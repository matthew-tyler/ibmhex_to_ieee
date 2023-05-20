#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

#define SHORT_WIDTH 24
#define LONG_WIDTH 56

/*
 * |------------------------ IBM_FLOAT Union Representation ------------------------|
 *
 *
 *  For the purposes of conversion one union is used to represent both single prescision (IBM Short)
 *  and double precision (IBM Long) formats.
 *
 *  The data type can be used by setting the number field with either 32 or 64 bits, the sign and
 *  exponent remain the same between the two formats, fraction is considered to be the short (24 bits)
 *  form of the fraction, while long_fraction has its own field.
 *
 *  Note: The diagram is written as big endian as is standard for the IBM System/360, and also just looks
 *        better for the diagram imo. The union itself is set out in little endian as is more common.
 *        The program will assume the input data is Big Endian as the etude says we are converting data
 *        from an IBM System, but will also attempt to determine this at run time.
 *
 * Note: Each '-' denotes a bit.
 *
 *                                     number
 *                         |------------------------------------------------------------------------|
 *                         |                                 64 bits                                 |
 *                         |------------------------------------------------------------------------|
 *
 *                                     struct
 *                         |------------------------------------------------------------------------|
 *                         | 1 bit | 7 bits |           24 bits            |         32 bits        |
 *                         |-------|--------|-----------------------------|------------------------|
 *                         | sign  |exponent|          fraction           |      Not used in struct|
 *                         |------------------------------------------------------------------------|
 *
 *                               long_fraction
 *                         |------------------------------------------------------------------------|
 *                         | 8 bits |                        56 bits                                 |
 *                         |--------|----------------------------------------------------------------|
 *                         |Not used|                         long_fraction                          |
 *                         |------------------------------------------------------------------------|
 *
 */

typedef union
{
    uint64_t number; // Holds 64bits. Set this value with the raw binary stream.
    struct
    {
        uint32_t fraction : 24; // The fraction of a Short IBM float
        uint8_t exponent : 7;   // The exponent and sign bit are the same for both short and long.
        uint8_t sign : 1;
    } __attribute__((packed));   // For the compiler to not add any padding. Should work for gcc, might need to add more?
    uint64_t long_fraction : 56; // the fraction of a Long IBM float

} IBM_FLOAT;

void checkError(int rs, char *msg)
{
    if (rs < 0)
    {
        perror(msg);
        exit(rs);
    }
}

int main(int argc, char const *argv[])
{

    if (argc != 3)
    {
        printf("Usage: floating [FILE] [-s] or [-d]\n");
        exit(0);
    }

    FILE *file = fopen(argv[1], "rb");

    if (file == NULL)
    {
        printf("Failed to open file\n");
        return 1;
    }

    int precision = 0;

    if (strcmp(argv[2], "-s") == 0)
    {
        precision = 32;
    }
    else if (strcmp(argv[2], "-d") == 0)
    {
        precision = 64;
    }
    else
    {
        printf("Usage: floating [FILE] [-s] or [-d]\n");
        exit(0);
    }

    unsigned char buffer[precision];

    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
    }

    if (ferror(file))
    {
        printf("Error reading from file\n");
    }

    fclose(file);
    return 0;
}
