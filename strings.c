#include <stdlib.h>
#include <string.h>

#include "strings.h"


char *str_replace(char *orig, char *find, char *rep, bool free_orig)
{
    char *tmp = orig;
    unsigned int count;

    size_t find_len = strlen(find);
    size_t rep_len = strlen(rep);

    for(count = 0; tmp = strstr(tmp, find); count++, tmp+=find_len);

    char * result = tmp = malloc(strlen(orig) + count * (rep-find) + 1);
    char * bak = orig;

    while(count--) {
        size_t found_len = strstr(orig, find) - orig;
        tmp = strncpy(tmp, orig, found_len) + found_len;
        tmp = strncpy(tmp, rep, rep_len) + rep_len;
        orig += found_len + find_len;
    }
    strcpy(tmp, orig);
    if(free_orig)
        free(bak);
    return result;
}
