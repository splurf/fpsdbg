#include "../inc/err.h"

void error(const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void _error(int _, const char *description) {
    error(description);
}