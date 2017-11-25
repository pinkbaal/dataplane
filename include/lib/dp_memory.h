#ifndef __US_MEMORY_H__
#define __US_MEMORY_H__


void *dp_malloc(unsigned long val, unsigned int size);

void *dp_zalloc(unsigned long val, unsigned int size);

void dp_free(void *);

#endif //__US_MEMORY_H__
