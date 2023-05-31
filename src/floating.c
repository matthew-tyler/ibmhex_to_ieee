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
#include <float.h>

#define EXPONENT_BIAS 64
#define SHORT_WIDTH 24
#define LONG_WIDTH 56
#define BASE16_TO_BASE2 4
#define SHORT 4
#define LONG 8

//  _______ __               __   __               ______         __         __
// |    ___|  |.-----.---.-.|  |_|__|.-----.-----.|   __ \.-----.|__|.-----.|  |_
// |    ___|  ||  _  |  _  ||   _|  ||     |  _  ||    __/|  _  ||  ||     ||   _|
// |___|   |__||_____|___._||____|__||__|__|___  ||___|   |_____||__||__|__||____|
//                                         |_____|

/*
 * |------------------------ IBM_FLOAT Union Representation ------------------------|
 *
 *
 *  For the purposes of conversion one union is used to represent both single precision (IBM Short)
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
 *                         |                                 64 bits                                |
 *                         |------------------------------------------------------------------------|
 *
 *                                                     short view
 *                         |------------------------------------------------------------------------|
 *                         | 1 bit | 7 bits |           24 bits            |         32 bits        |
 *                         |-------|--------|-----------------------------|-------------------------|
 *                         | sign  |exponent|          fraction           |      Not used in short  |
 *                         |------------------------------------------------------------------------|
 *
 *                                              long view (l is long_ in code)
 *                         |------------------------------------------------------------------------|
 *                         | 1 bit | 7 bits |           56 bits                                     |
 *                         |-------|--------|-------------------------------------------------------|
 *                         |lsign |lexponent|         lfraction                                     |
 *                         |------------------------------------------------------------------------|
 *
 *
 */
#pragma pack(push, 1) // Should hopefully cover struct packing on windows ...
typedef union
{
    uint64_t number; // Holds 64bits. Set this value with the raw binary stream.
    struct
    {
        uint32_t fraction : 24; // The fraction of a Short IBM float
        uint8_t exponent : 7;   // The exponent of the short ibm
        uint8_t sign : 1;       // Short sign. 0 for positive, 1 for negative.
    } __attribute__((packed));  // For the compiler to not add any padding. Should work for gcc, might need to add more?
    struct
    {
        uint64_t long_fraction : 56; // the fraction of a Long IBM float
        uint8_t long_exponent : 7;   // exponent for a long ibm
        uint8_t long_sign : 1;       // exponent for al ong sign
    } __attribute__((packed));

} IBM_FLOAT;
#pragma pack(pop)

/**********************************************
 *          ibm32_to_ieee
 **********************************************
 *
 *
 *   IBM Formula (−1)^sign *  16^(exponent - 64) * 0.significand
 *   IEEE Formula (-1)^sign * 2^(exponent - bias) * 1.mantissa
 *
 *   16^(exponent - 64) to 2^(exponent - bias)
 *   a^(bc) = (a^b)^c
 *   16^(exponent - bias) = (2^4)^(exponent - bias) = (2^4)^(exponent - bias) = 2^(4*(exponent - bias))
 *
 *   0.significand * 2^-24  == exponent - 24;
 *
 *   - Subtract the exponent bias from the IBM exponent to adjust the range.
 *   - Multiply the difference (exponent - bias) by the conversion factor BASE16_TO_BASE2 to convert the exponent from base 16 to base 2.
 *   - Subtract the SHORT_WIDTH (in base 2), same as multiplying the significand by 2^-24(56 for long) to remove assumed 1.
 *
 *  @brief Converts an 32bit IBM Float to IEEE Double
 *  @param  number to convert
 *  @return IEEE floating point representation.
 */
double ibm32_to_ieee(IBM_FLOAT num)
{
    int exponent = ((num.exponent - EXPONENT_BIAS) * BASE16_TO_BASE2) - SHORT_WIDTH;
    double ieenum = ldexp(num.fraction, exponent) * pow(-1, num.sign); // ldexp is apparently a better way to do x * 2^n
    return ieenum;
}

/**********************************************
 *          ibm64_to_ieee
 **********************************************
 *  @brief Converts an 64bit IBM Float to IEEE Double
 *  @param  number to convert
 *  @return IEEE floating point representation.
 */
double ibm64_to_ieee(IBM_FLOAT num)
{
    int exponent = ((num.long_exponent - EXPONENT_BIAS) * BASE16_TO_BASE2) - LONG_WIDTH;
    double ieenum = ldexp(num.long_fraction, exponent) * pow(-1, num.long_sign);
    return ieenum;
}

/**********************************************
 *          print_ibm_float_decimal
 **********************************************
 *
 *  In principal the same as the other approach. The original way I went about it.
 *  I think it is less accurate but don't have easy ways to test.
 *
 *  It's just a direct conversion based off of the formulas, just directly converts to decimal and back,
 *  but according to ChatGPT and some research it has a good chance of losing accuracy, hence using ldexp
 *  from the math.h library for more accurate conversion (and some free error handling!).
 *
 *  This currently just prints the number, but obviously can be used to parse the textual output back
 *  into IEEE.
 *
 *
 *  @brief Converts and prints ibm floats. Might be less accurate than other approach due to loss of precision
 *  @param  f the IBM_FLOAT to print
 */
void print_ibm_float_decimal(IBM_FLOAT f, int precision)
{

    int width = SHORT_WIDTH;

    if (precision == LONG)
    {
        width = LONG_WIDTH;
    }

    double exponent_part = pow(16, (f.exponent - EXPONENT_BIAS));
    double fraction_part = f.fraction / pow(2, width);

    double number = pow(-1, f.sign) * exponent_part * fraction_part;

    printf("%lf\n", number);
}

/**********************************************
 *          is_little_endian
 **********************************************
 *
 *  Generated by ChatGPT, seems to work.
 *  In principle just defining a number that spans two bytes
 *  then checking the high byte to see if it is set to 1 or not.
 *
 *  @brief simple runtime check for system endianess
 *  @return True for little endian, else false
 */
bool is_little_endian()
{
    uint16_t number = 0x1;
    char *numPtr = (char *)&number;
    return (numPtr[0] == 1);
}

/**********************************************
 *          reverse_buffer
 **********************************************
 *
 *  Another quick GPT jobby. Reverses the order of the buffer
 *  if needed. Assumes input data is in big endian, which should be for IBM System/360 data
 *
 *  @brief Reverses endianess of buffer if required, assumes buffer is big endian
 *  @param buffer
 *  @param size
 */
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

int main(int argc, char const *argv[])
{

    char input_filename[30];
    char output_filename[30];
    char input_type[2];
    char output_type[2];

    printf("Enter name of file to read from: ");
    scanf("%29s", input_filename);

    FILE *input_file = fopen(input_filename, "rb"); // open for reading in binary mode
    if (input_file == NULL)
    {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    printf("Enter input type ('s' for single point or 'd' for double point): ");
    scanf("%1s", input_type);

    int input_precision = 0;

    if (strcmp(input_type, "s") == 0)
    {
        input_precision = SHORT;
    }
    else if (strcmp(input_type, "d") == 0)
    {
        input_precision = LONG;
    }
    else
    {
        perror("Error, enter s or d");
        return EXIT_FAILURE;
    }

    printf("Enter name of output file: ");
    scanf("%29s", output_filename);

    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL)
    {
        perror("Error opening output file");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    printf("Enter output type ('s' for single point or 'd' for double point): ");
    scanf("%1s", output_type);

    int output_precision = 0;

    if (strcmp(output_type, "s") == 0)
    {
        output_precision = SHORT;
    }
    else if (strcmp(output_type, "d") == 0)
    {
        output_precision = LONG;
    }
    else
    {
        perror("Error, enter s or d");
        return EXIT_FAILURE;
    }

    IBM_FLOAT to_convert;
    unsigned char buffer[input_precision];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), input_file)) > 0)
    {

        double ieee = 1.0;
        reverse_buffer(buffer, input_precision);

        if (input_precision == SHORT)
        {
            memcpy(&to_convert.number, buffer, SHORT);
            ieee = ibm32_to_ieee(to_convert);
        }
        else
        {
            memcpy(&to_convert.number, buffer, LONG);
            ieee = ibm64_to_ieee(to_convert);
        }

        if (output_precision == SHORT)
        {

            if (ieee == HUGE_VAL || ieee > FLT_MAX || ieee < -FLT_MAX)
            {
                ieee = 0;
            }

            float float_ieee = (float)ieee;
            fwrite(&float_ieee, sizeof(float), 1, output_file);
        }
        else
        {
            if (ieee == HUGE_VAL)
            {
                ieee = 0;
            }
            fwrite(&ieee, sizeof(double), 1, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);

    return EXIT_SUCCESS;
}
