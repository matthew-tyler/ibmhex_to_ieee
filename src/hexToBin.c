#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void replaceSubstring(char* str, const char* find, const char* replace) {
    int find_len = strlen(find);
    int replace_len = strlen(replace);

    char* found_at;

    while ((found_at = strstr(str, find)) != NULL) {
        memmove(found_at + replace_len, found_at + find_len, strlen(found_at + find_len) + 1);
        memcpy(found_at, replace, replace_len);
    }
}

int main(int argc, char *argv[]){
    if (argc != 3)
    {
        printf("Usage: hexToBin [FILE] [-s] or [-d]\n");
        exit(0);
    }
    
    FILE* hex_file = fopen(argv[1], "r");
    if (hex_file == NULL) {
        printf("Failed to open the hex file.\n");
        return 1; // or handle the error accordingly
    }

    char* fileName = argv[1];
    const char* find = "hex";
    const char* replace = "bin";
    replaceSubstring(fileName, find, replace);

    FILE* binary_file = fopen(fileName, "wb");
    if (binary_file == NULL) {
        printf("Failed to open the binary file.\n");
        fclose(hex_file);
        return 1; // or handle the error accordingly
    }

    if(strcmp(argv[2], "-s") == 0){
        float single_value;
        while (fscanf(hex_file, "%lx", &single_value) != EOF) {
            fwrite(&single_value, sizeof(float), 1, binary_file);
        }
    }
    else if(strcmp(argv[2], "-d") == 0){
        double double_value;
        while (fscanf(hex_file, "%lx", &double_value) != EOF) {
            fwrite(&double_value, sizeof(double), 1, binary_file);
        }
    }
    else{
        printf("Usage: hexToBin [FILE] [-s] or [-d]\n");
        exit(0);
    }
    
    fclose(hex_file);
    fclose(binary_file);

    return 0;
}