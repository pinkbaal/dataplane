#ifndef __DP_MEMORY_H__
#define __DP_MEMORY_H__


void *dp_malloc(unsigned long val, unsigned int size);

void *dp_zalloc(unsigned long val, unsigned int size);

void dp_free(void *);

#endif //__DP_MEMORY_H__
