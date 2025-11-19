#include "utils.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char* utils_trim(char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    
    while (start <= end && isspace(str[start])) start++;
    while (end >= start && isspace(str[end])) end--;
    
    str[end + 1] = '\0';
    return str + start;
}

int utils_file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}