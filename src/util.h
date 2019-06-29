#ifndef __UTIL_H__
#define __UTIL_H__

#include "definitions.h"

int util_write_array(precision *data, int N, int dim, const char *dir, const char *varName);
void util_create_dir(const char *directory);

#endif // __UTIL_H__
