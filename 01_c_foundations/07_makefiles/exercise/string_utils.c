/*
String Utilities Implementation
*/

#include <ctype.h>
#include "string_utils.h"

void to_upper(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int string_length(const char *str) {
    int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}
