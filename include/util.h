/* Simple IO handling, including a method for reading the contents of a file */

/**
 * @file
 * @author Evan Schwartzentruber
*/

#include <stdlib.h>
#include "err.h"

/** Return a buffer of the contents of the file at the provided path */
char *read_file(char *path) {
    FILE *f = fopen(path, "r");

    if (!f) {
        error("File not found");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    size_t len = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = (char *)malloc(len);
    size_t bytes = fread(buffer, 1, len, f);
    return bytes > 0 ? buffer : NULL;
}