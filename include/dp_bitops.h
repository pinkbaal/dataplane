#ifndef __DP_BITOPS_H__
#define __DP_BITOPS_H__

#include <dp_types.h>

#define BITS_PER_ULONG	(sizeof(u64) << 3)
#define SHIFT_PER_ULONG	(((1 << 5) == BITS_PER_ULONG) ? 5 : 6)
#define BITS_MASK(idx)	((u64)1 << ((idx) & (BITS_PER_ULONG - 1)))
#define BITS_IDX(idx)	((idx) >> SHIFT_PER_ULONG)
#define BITS_TO_LONGS(bits) (((bits)+BITS_PER_ULONG-1)/BITS_PER_ULONG)


static inline u64 test_bits(u64 mask, volatile u64 *p)
{
	return *p & mask;
}

static inline s32 test_bit(int idx, volatile u64 *bits)
{
	return test_bits(BITS_MASK(idx), bits + BITS_IDX(idx));
}

static inline void set_bits(u64 mask, volatile u64 *p)
{
	*p |= mask;
}

static inline void set_bit(s32 idx, volatile u64 *bits)
{
	set_bits(BITS_MASK(idx), bits + BITS_IDX(idx));
}

static inline void clear_bits(u64 mask, volatile u64 *p)
{
	*p &= ~mask;
}

static inline void clear_bit(s32 idx, volatile u64 *bits)
{
	clear_bits(BITS_MASK(idx), bits + BITS_IDX(idx));
}


static inline u64 test_and_set_bits(u64 mask, volatile u64 *p)
{
	u64 ret = test_bits(mask, p);

	set_bits(mask, p);
	return ret;
}

static inline s32 test_and_set_bit(s32 idx, volatile u64 *bits)
{
	int ret = test_bit(idx, bits);

	set_bit(idx, bits);
	return ret;
}

static inline s32 test_and_clear_bit(int idx, volatile u64 *bits)
{
	s32 ret = test_bit(idx, bits);

	clear_bit(idx, bits);
	return ret;
}

static inline s32 find_next_zero_bit(u64 *bits, s32 limit, s32 idx)
{
	while ((++idx < limit) && test_bit(idx, bits))
		;
	return idx;
}

static inline s32 find_first_zero_bit(u64 *bits, s32 limit)
{
	s32 idx = 0;

	while (test_bit(idx, bits) && (++idx < limit))
		;
	return idx;
}



#endif //__DP_BITOPS_H__

