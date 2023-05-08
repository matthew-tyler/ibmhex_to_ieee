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

    int file_descriptor = open(argv[1], O_RDONLY);
    checkError(file_descriptor, "Failed to open file");

    int precision;

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

    return 0;
}
