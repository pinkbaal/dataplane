
#include <rte_malloc.h>

void *dp_malloc(unsigned long val, size_t size)
{
	(void)val;
	return rte_malloc("dp_memmory", size, 0);
}

void *dp_zalloc(unsigned long val, size_t size)
{
	(void)val;
	return rte_zmalloc("dp_memmory", size, 0);
}

void *dp_realloc(unsigned long val, void *ptr, size_t size)
{
	(void)val;
	return rte_realloc(ptr, size, 0);
}


void dp_free(void *ptr)
{
	rte_free(ptr);
}







