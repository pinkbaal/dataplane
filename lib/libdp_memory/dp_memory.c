
#include <rte_malloc.h>

void *dp_malloc(unsigned long val, unsigned int size)
{
	(void)val;
	return rte_malloc("dp_memmory", size, 0);
}

void *dp_zalloc(unsigned long val, unsigned int size)
{
	(void)val;
	return rte_zmalloc("dp_memmory", size, 0);
}

void dp_free(void *ptr)
{
	rte_free(ptr);
}

