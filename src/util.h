#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h>
#include <stdbool.h>

#include "definitions.h"

typedef struct str_builder str_builder_t;

int util_write_array_precision(precision *data, int N, int dim, const char *dir,
                               const char *chain_type, const char *varName);
int util_write_array_int(int *data, int N, int dim, const char *dir,
                         const char *chain_type, const char *varName);
/* Recursive directory creator */
bool rw_create_dir(const char *name);

/* Create a str builder. */
str_builder_t *str_builder_create(void);
/* Destroy a str builder. */
void str_builder_destroy(str_builder_t *sb);
/* Add a string to the builder. */
void str_builder_add_str(str_builder_t *sb, const char *str, size_t len);
/* Add a character to the builder. */
void str_builder_add_char(str_builder_t *sb, char c);
/* Add an integer as to the builder. */
void str_builder_add_int(str_builder_t *sb, int val);
/* Clear the builder. */
void str_builder_clear(str_builder_t *sb);
/* Remove data from the end of the builder. */
void str_builder_truncate(str_builder_t *sb, size_t len);
/* Remove data from the beginning of the builder. */
void str_builder_drop(str_builder_t *sb, size_t len);
/* The length of the string contained in the builder. */
size_t str_builder_len(const str_builder_t *sb);
/* A pointer to the internal buffer with the builder's string data. */
const char *str_builder_peek(const str_builder_t *sb);
/* Return a copy of the string data. */
char *str_builder_dump(const str_builder_t *sb, size_t *len);
/* String splitting function */
char **str_split(const char *in, size_t in_len, char delm, size_t *num_elm, size_t max);
/* Free splitting function */
void str_split_free(char **in, size_t num_elm);
#endif /* __UTIL_H__ */
