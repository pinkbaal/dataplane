
#include <rte_malloc.h>

void *__dp_malloc(size_t size)
{
	return rte_malloc("dp_memmory", size, 0);
}

void *__dp_zalloc(size_t size)
{
	return rte_zmalloc("dp_memmory", size, 0);
}

void *__dp_realloc(void *ptr, size_t size)
{
	return rte_realloc(ptr, size, 0);
}


void __dp_free(void *ptr)
{
	rte_free(ptr);
}







