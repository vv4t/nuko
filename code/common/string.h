/*
-- string.h --

String manipulation functions.

NOTE: this is only sparingly used in cmd.c. Perhaps remove?
*/

#ifndef STRING_H
#define STRING_H

#include "zone.h"
#include <string.h>

// Allocate a buffer and copy the string into it
// Used to preserve stack-allocated strings
char *string_copy(const char *str);

#endif
