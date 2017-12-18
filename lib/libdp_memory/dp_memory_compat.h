#ifndef __DP_MEMORY_COMPAT_H__
#define __DP_MEMORY_COMPAT_H__

#include <dp_types.h>


void *__dp_malloc(size_t size);
void *__dp_zalloc(size_t size);
void *__dp_realloc(void *ptr, size_t size);
void __dp_free(void *ptr);
s8 *__dp_strdup(const s8 *str);
void *__dp_calloc(size_t nmemb, size_t size);

#endif //__DP_MEMORY_COMPAT_H__
