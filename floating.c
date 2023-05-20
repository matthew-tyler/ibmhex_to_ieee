#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

void parse_file(char *buffer, FILE *file)
{
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

    FILE *file = fopen(argv[1], "rb"); // Open a binary file for reading
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
