#ifndef __SHERO_HTTP11_COMMON_H
#define __SHERO_HTTP11_COMMON_H

#include <sys/types.h>

typedef void (*element_cb)(void *data, const char *at, size_t length);
typedef void (*field_cb)(void *data, const char *field, size_t flen, const char *value, size_t vlen);


#endif
