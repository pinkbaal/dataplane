
#include <stdio.h>

#include <dp_types.h>

#include <lib/dp_memory.h>
#include <lib/dp_vector.h>

s32 main(s32 argc, s8 **argv)
{
	void *ptr = dp_malloc(0, 128);
	vector_s *v = vector_init(10);

	printf("%p, %p\n", ptr, v);

	dp_free(ptr);

	return 0;
}

