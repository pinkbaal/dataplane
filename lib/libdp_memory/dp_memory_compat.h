#ifndef __DP_MEMORY_COMPAT_H__
#define __DP_MEMORY_COMPAT_H__

void *__dp_malloc(size_t size);
void *__dp_zalloc(size_t size);
void *__dp_realloc(void *ptr, size_t size);
void __dp_free(void *ptr);

#endif //__DP_MEMORY_COMPAT_H__
