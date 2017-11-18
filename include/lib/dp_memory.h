#ifndef __US_MEMORY_H__
#define __US_MEMORY_H__


void *us_malloc(unsigned long val, unsigned int size);

void *us_zalloc(unsigned long val, unsigned int size);

void us_free(void *);

#endif //__US_MEMORY_H__
