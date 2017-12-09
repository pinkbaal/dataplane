
#include <stdlib.h>
#include <string.h>

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







