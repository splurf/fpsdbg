/// Simple error handling, including the error callback method.
/// @file
/// @author Evan Schwartzentruber

#include <stdio.h>

void error(const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void _error(int _, const char *description) {
    error(description);
}