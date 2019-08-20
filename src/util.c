/*****************************************************************************
 *
 *  util.c
 *
 * Efficient C String Builder
 * taken from https://nachtimwald.com/2017/02/26/efficient-c-string-builder/
 * and Recursive Create Directory in C
 * taken from https://nachtimwald.com/2017/05/17/recursive-create-directory-in-c/
 *
 *****************************************************************************/

#ifdef _WIN32
const char SEP = '\\';
#else
const char SEP = '/';
#endif

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "util.h"

static const size_t str_builder_min_size = 32;

struct str_builder {
    char   *str;
    size_t  alloced;
    size_t  len;
};


static void createheader(char *header, int dim, const char *varName);

/*****************************************************************************
 *
 *  util_write_array_precision
 *
 *****************************************************************************/

int util_write_array_precision(precision *data, int N, int dim, const char *dir,
                               const char *chain_type, const char *varName){

  char filename[BUFSIZ], header[BUFSIZ];
  FILE *fp = NULL;

  sprintf(filename, "%s/%s_%s.csv", dir, chain_type, varName);

  printf("%30s\t%50s", "Writing:",filename);
  fp = fopen(filename, "w+");
  assert(fp);

  createheader(header, dim, varName);

  /* write header */
  fprintf(fp, "%s\n", header);

  int i,j;
  for(i=0; i<N; i++)
  {
    for(j=0; j<dim-1; j++)
    {
      fprintf(fp, "%.*e,", PRINT_PREC-1,data[i*dim+j]);
    }
    fprintf(fp, "%.*e\n", PRINT_PREC-1, data[i*dim+dim-1]);
  }

  fclose(fp);
  printf("\tDone\n");

  return 0;
}

/*****************************************************************************
 *
 *  util_write_array_int
 *
 *****************************************************************************/

int util_write_array_int(int *data, int N, int dim, const char *dir,
                         const char *chain_type, const char *varName){

  char filename[BUFSIZ], header[BUFSIZ];
  FILE *fp;

  sprintf(filename, "%s/%s_%s.csv", dir, chain_type, varName);

  printf("%30s\t%50s", "Writing:",filename);
  fp = fopen(filename, "w+");
  assert(fp);

  createheader(header, dim, varName);

  /* write header */
  fprintf(fp, "%s\n", header);

  int i,j;
  for(i=0; i<N; i++)
  {
    for(j=0; j<dim-1; j++)
    {
      fprintf(fp, "%d,", data[i*dim+j]);
    }

    fprintf(fp, "%d\n", data[i*dim+dim-1]);
  }

  fclose(fp);
  printf("\tDone\n");

  return 0;
}

/*****************************************************************************
 *
 *  rw_create_dir
 *
 *****************************************************************************/

bool rw_create_dir(const char *name)
{
    str_builder_t  *sb;
    char          **parts;
    size_t          num_parts;
    size_t          i;
    bool            ret = true;

    if (name == NULL || *name == '\0')
        return false;

    parts = str_split(name, strlen(name), SEP, &num_parts, 0);
    if (parts == NULL || num_parts == 0) {
        str_split_free(parts, num_parts);
        return false;
    }

    sb = str_builder_create();
    i  = 0;
#ifdef _WIN32
    /* If the first part has a ':' it's a drive. E.g 'C:'. We don't
     * want to try creating it because we can't. We'll add it to base
     * and move forward. The next part will be a directory we need
     * to try creating. */
    if (strchr(parts[0], ':')) {
        i++;
        str_builder_add_str(sb, parts[0], strlen(parts[0]));
        str_builder_add_char(sb, SEP);
    }
#else
    if (*name == '/') {
        str_builder_add_char(sb, SEP);
    }
#endif

    for ( ; i<num_parts; i++) {
        if (parts[i] == NULL || *(parts[i]) == '\0') {
            continue;
        }

        str_builder_add_str(sb, parts[i], strlen(parts[i]));
        str_builder_add_char(sb, SEP);

#ifdef _WIN32
        if (CreateDirectory(str_builder_peek(sb), NULL) == FALSE) {
            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                ret = false;
                goto done;
            }
        }
#else
        if (mkdir(str_builder_peek(sb), 0774) != 0)
            if (errno != EEXIST) {
                ret = false;
                goto done;
            }
#endif
    }

done:
    str_split_free(parts, num_parts);
    str_builder_destroy(sb);
    return ret;
}

str_builder_t *str_builder_create(void)
{
    str_builder_t *sb;

    sb          = calloc(1, sizeof(*sb));
    sb->str     = malloc(str_builder_min_size);
    *sb->str    = '\0';
    sb->alloced = str_builder_min_size;
    sb->len     = 0;

    return sb;
}

void str_builder_destroy(str_builder_t *sb)
{
    if (sb == NULL)
        return;
    free(sb->str);
    free(sb);
}

/* - - - - */

/*! Ensure there is enough space for data being added plus a NULL terminator.
 *
 * param[in,out] sb      Builder.
 * param[in]     add_len The length that needs to be added *not* including a NULL terminator.
 */
static void str_builder_ensure_space(str_builder_t *sb, size_t add_len)
{
    if (sb == NULL || add_len == 0)
        return;

    if (sb->alloced >= sb->len+add_len+1)
        return;

    while (sb->alloced < sb->len+add_len+1) {
        /* Doubling growth strategy. */
        sb->alloced <<= 1;
        if (sb->alloced == 0) {
            /* Left shift of max bits will go to 0. An unsigned type set to
             * -1 will return the maximum possible size. However, we should
             *  have run out of memory well before we need to do this. Since
             *  this is the theoretical maximum total system memory we don't
             *  have a flag saying we can't grow any more because it should
             *  be impossible to get to this point. */
            sb->alloced--;
        }
    }
    sb->str = realloc(sb->str, sb->alloced);
}

/* - - - - */

void str_builder_add_str(str_builder_t *sb, const char *str, size_t len)
{
    if (sb == NULL || str == NULL || *str == '\0')
        return;

    if (len == 0)
        len = strlen(str);

    str_builder_ensure_space(sb, len);
    memmove(sb->str+sb->len, str, len);
    sb->len += len;
    sb->str[sb->len] = '\0';
}

void str_builder_add_char(str_builder_t *sb, char c)
{
    if (sb == NULL)
        return;
    str_builder_ensure_space(sb, 1);
    sb->str[sb->len] = c;
    sb->len++;
    sb->str[sb->len] = '\0';
}

void str_builder_add_int(str_builder_t *sb, int val)
{
    char str[12];

    if (sb == NULL)
        return;

    snprintf(str, sizeof(str), "%d", val);
    str_builder_add_str(sb, str, 0);
}

/* - - - - */

void str_builder_clear(str_builder_t *sb)
{
    if (sb == NULL)
        return;
    str_builder_truncate(sb, 0);
}

void str_builder_truncate(str_builder_t *sb, size_t len)
{
    if (sb == NULL || len >= sb->len)
        return;

    sb->len = len;
    sb->str[sb->len] = '\0';
}

void str_builder_drop(str_builder_t *sb, size_t len)
{
    if (sb == NULL || len == 0)
        return;

    if (len >= sb->len) {
        str_builder_clear(sb);
        return;
    }

    sb->len -= len;
    /* +1 to move the NULL. */
    memmove(sb->str, sb->str+len, sb->len+1);
}

/* - - - - */

size_t str_builder_len(const str_builder_t *sb)
{
    if (sb == NULL)
        return 0;
    return sb->len;
}

const char *str_builder_peek(const str_builder_t *sb)
{
    if (sb == NULL)
        return NULL;
    return sb->str;
}

char *str_builder_dump(const str_builder_t *sb, size_t *len)
{
    char *out;

    if (sb == NULL)
        return NULL;

    if (len != NULL)
        *len = sb->len;
    out = malloc(sb->len+1);
    memcpy(out, sb->str, sb->len+1);
    return out;
}

char **str_split(const char *in, size_t in_len, char delm, size_t *num_elm, size_t max)
{
    char   *parsestr;
    char   **out;
    size_t  cnt = 1;
    size_t  i;

    if (in == NULL || in_len == 0 || num_elm == NULL)
        return NULL;

    parsestr = malloc(in_len+1);
    memcpy(parsestr, in, in_len+1);
    parsestr[in_len] = '\0';

    *num_elm = 1;
    for (i=0; i<in_len; i++) {
        if (parsestr[i] == delm)
            (*num_elm)++;
        if (max > 0 && *num_elm == max)
            break;
    }

    out    = malloc(*num_elm * sizeof(*out));
    out[0] = parsestr;
    for (i=0; i<in_len && cnt<*num_elm; i++) {
        if (parsestr[i] != delm)
            continue;

        /* Add the pointer to the array of elements */
        parsestr[i] = '\0';
        out[cnt] = parsestr+i+1;
        cnt++;
    }

    return out;
}

void str_split_free(char **in, size_t num_elm)
{
    if (in == NULL)
        return;
    if (num_elm != 0)
        free(in[0]);
    free(in);
}

/*****************************************************************************
 *
 *  createheader
 *
 *****************************************************************************/

static void createheader(char *header, int dim, const char *varName){

  int i;
  char temp[BUFSIZ];

  strcpy(header, "#");
  for(i=0; i<dim-1; i++)
  {
    sprintf(temp, "%s%d,", varName, i);
    strcat(header, temp);
  }
  sprintf(temp, "%s%d,", varName, dim-1);
  strcat(header, temp);
}
