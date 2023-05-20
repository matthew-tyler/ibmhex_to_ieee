#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>

#define EXPONENT_BIAS 64
#define SHORT_WIDTH 24
#define LONG_WIDTH 56
#define BASE16_TO_BASE2 4
#define SHORT 4
#define LONG 8

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
        uint32_t fraction : 24;  // The fraction of a Short IBM float
        uint8_t exponent : 7;    // The exponent and sign bit are the same for both short and long.
        uint8_t sign : 1;        // Same for both. 0 for postive, 1 for negative.
    } __attribute__((packed));   // For the compiler to not add any padding. Should work for gcc, might need to add more?
    uint64_t long_fraction : 56; // the fraction of a Long IBM float

} IBM_FLOAT;

/*

    IBM Formula (−1)^sign *  16^(exponent - 64) * 0.significand
    IEEE Formula (-1)^sign * 2^(exponent - bias) * 1.mantissa

    16^(exponent - 64) to 2^(exponent - bias)
    a^(bc) = (a^b)^c
    16^(exponent - bias) = (2^4)^(exponent - bias) = (2^4)^(exponent - bias) = 2^(4*(exponent - bias))

    0.significand * 2^-24  == exponent - 24;

    - Subtract the exponent bias from the IBM exponent to adjust the range.
    - Multiply the difference (exponent - bias) by the conversion factor BASE16_TO_BASE2 to convert the exponent from base 16 to base 2.
    - Subtract the SHORT_WIDTH (in base 2)


*/
double ibm32_to_ieee(IBM_FLOAT num)
{
    int exponent = ((num.exponent - EXPONENT_BIAS) * BASE16_TO_BASE2) - SHORT_WIDTH;
    double ieenum = ldexp(num.fraction, exponent) * pow(-1, num.sign); // ldexp is apparently a better way to do x * 2^n
    return ieenum;
}

double ibm64_to_ieee(IBM_FLOAT num)
{
    int exponent = ((num.exponent - EXPONENT_BIAS) * BASE16_TO_BASE2) - LONG_WIDTH;
    double ieenum = ldexp(num.long_fraction, exponent) * pow(-1, num.sign);
    return ieenum;
}

void print_ibm_float_decimal(IBM_FLOAT f, int precision)
{

    int width = SHORT_WIDTH;

    if (precision == LONG)
    {
        width = LONG_WIDTH;
    }

    double exponent_part = pow(16, (f.exponent - EXPONENT_BIAS));
    double fraction_part = f.fraction / pow(2, 24);

    double number = pow(-1, f.sign) * exponent_part * fraction_part;

    printf("%lf\n", number);
}

bool is_little_endian()
{
    uint16_t number = 0x1;
    char *numPtr = (char *)&number;
    return (numPtr[0] == 1);
}

void reverse_buffer(unsigned char *buffer, int size)
{
    if (is_little_endian())
    {
        int start = 0;
        int end = size - 1;
        while (start < end)
        {
            unsigned char temp = buffer[start];
            buffer[start] = buffer[end];
            buffer[end] = temp;
            start++;
            end--;
        }
    }
}

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
        precision = SHORT;
    }
    else if (strcmp(argv[2], "-d") == 0)
    {
        precision = LONG;
    }
    else
    {
        printf("Usage: floating [FILE] [-s] or [-d]\n");
        exit(0);
    }

    IBM_FLOAT to_convert;

    unsigned char buffer[precision];

    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {

        double ieee = 1.0;
        reverse_buffer(buffer, precision);
        if (precision == SHORT)
        {
            memcpy(&to_convert.number, buffer, SHORT);
            ieee = ibm32_to_ieee(to_convert);
            print_ibm_float_decimal(to_convert, precision);
        }
        else
        {
            memcpy(&to_convert.number, buffer, LONG);
            to_convert.number = *((uint64_t *)buffer);
            ieee = ibm64_to_ieee(to_convert);
        }
        printf("%lf\n", ieee);
    }

    if (ferror(file))
    {
        printf("Error reading from file\n");
    }

    fclose(file);
    return 0;
}
