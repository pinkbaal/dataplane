#include <stdlib.h>
#include <string.h>

#include "dp_memory_compat.h"

__attribute__((weak)) void *__dp_malloc(size_t size)
{
	return malloc(size);
}

__attribute__((weak)) void *__dp_zalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr) {
		memset(ptr, 0, size);
	}
	return ptr;
}

__attribute__((weak)) void *__dp_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

__attribute__((weak)) void __dp_free(void *ptr)
{
	return free(ptr);
}


__attribute__((weak)) void *__dp_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

