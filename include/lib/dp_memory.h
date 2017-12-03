#ifndef __DP_MEMORY_H__
#define __DP_MEMORY_H__


void *dp_malloc(unsigned long val, size_t size);

void *dp_zalloc(unsigned long val, size_t size);

void dp_free(void *);

void *dp_realloc(unsigned long val, void *ptr, size_t size);

#endif //__DP_MEMORY_H__
