/// Simple error handling, including the error callback method.
/// @file
/// @author Evan Schwartzentruber

#ifndef ERR_H
#define ERR_H

#include <stdio.h>

void error(const char *description);
void _error(int _, const char *description);

#endif