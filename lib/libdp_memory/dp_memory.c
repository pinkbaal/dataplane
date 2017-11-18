
#include <rte_malloc.h>

void *us_malloc(unsigned long val, unsigned int size)
{
	(void)val;
	return rte_malloc("us_memmory", size, 0);
}

void *us_zalloc(unsigned long val, unsigned int size)
{
	(void)val;
	return rte_zmalloc("us_memmory", size, 0);
}

void us_free(void *ptr)
{
	rte_free(ptr);
}

