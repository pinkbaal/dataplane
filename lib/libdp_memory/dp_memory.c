
#include <stdlib.h>
#include <string.h>

#include <lib/dp_memory.h>

#include "dp_memory_compat.h"


void *dp_malloc(unsigned long val, size_t size)
{
	(void)val;
	return __dp_malloc(size);
}

void *dp_zalloc(unsigned long val, size_t size)
{
	(void)val;
	return __dp_zalloc(size);
}

void *dp_realloc(unsigned long val, void *ptr, size_t size)
{
	(void)val;
	return __dp_realloc(ptr, size);
}

void dp_free(void *ptr)
{
	__dp_free(ptr);
}

void *dp_calloc(unsigned long val, size_t nmemb, size_t size)
{
	(void)val;
	return __dp_calloc(nmemb, size);
}









