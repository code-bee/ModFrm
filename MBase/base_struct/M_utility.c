/* 
	M_utility.c : 一组常用函数

	kevin.zheng@gmail.com
	2012/09/23
*/

#include "../MBase_priv.h"
#include "MBase.h"

typedef union p
{
	M_sint8* p8;
	M_sint16* p16;
	M_sint32* p32;

#ifdef __M_CFG_OS64
	M_sint64* p64;
#endif
} p_t;

INLINE	void M_snprintf(M_sint8* str, M_size_t size, const M_sint8* format, ...)
{
	va_list ap;
	M_sint32 n;

	if(size < 1)
		return;

	va_start(ap, format);
#ifdef __M_CFG_OS_WINDOWS
	_vsnprintf(str, size, format, ap);
	str[size-1] = 0;
#elif defined(__M_CFG_OS_LINUX)
	vsnprintf(str, size, format, ap);
#else
	#error "not supported OS!"
#endif
	va_end(ap);
}

INLINE	void M_swap(void* s1, void* s2, M_sintptr len)
{
	p_t p1, p2;
	M_sintptr i;
	M_sintptr tmp_len;

#ifdef __M_CFG_OS64
	tmp_len = len>>3;
	p1.p64 = (M_sint64*)s1;
	p2.p64 = (M_sint64*)s2;
	for(i=0; i < tmp_len; ++i)
	{
		M_swap64(p1.p64++, p2.p64++); 
	}

	p1.p32 = (M_sint32*)p1.p64;
	p2.p32 = (M_sint32*)p2.p64;
	if( (tmp_len = (len & 0x4) >> 2) )
	{
		M_swap32(p1.p32++, p2.p32++);
	}
	
#else
	tmp_len = len>>2;
	p1.p32 = (M_sint32*)s1;
	p2.p32 = (M_sint32*)s2;
	for(i=0; i < tmp_len; ++i)
	{
		M_swap32(p1.p32++, p2.p32++); 
	}
#endif

	p1.p16 = (M_sint16*)p1.p32;
	p2.p16 = (M_sint16*)p2.p32;
	if( (tmp_len = ((len & 0x2) >> 1)) )
	{
		M_swap16(p1.p16++, p2.p16++);
	}

	if( (tmp_len = (len & 0x1)) )
	{
		p1.p8 = (M_sint8*)p1.p16;
		p2.p8 = (M_sint8*)p2.p16;
		M_swap8(p1.p8++, p2.p8++);
	}
}

INLINE	void M_swap8(M_sint8* s1, M_sint8* s2)
{
	SWAP(*s1, *s2);
}
INLINE	void M_swap16(M_sint16* s1, M_sint16* s2)
{
	SWAP(*s1, *s2);
}
INLINE	void M_swap32(M_sint32* s1, M_sint32* s2)
{
	SWAP(*s1, *s2);
}
#ifdef __M_CFG_OS64
INLINE	void M_swap64(M_sint64* s1, M_sint64* s2)
{
	SWAP(*s1, *s2);
}
#endif
INLINE	void M_swapptr(M_sintptr* s1, M_sintptr* s2)
{
	SWAP(*s1, *s2);
}

/*
	prepare something for rand_int()
	s_rand_quotient: how many times of rand() will be called in rand_int()
	s_rand_remainder: how many remaind digits in random integer
	s_rand_digits: digits of RAND_MAX
*/
M_sint32	s_rand_quotient;
M_sint32	s_rand_remainder;
M_sint32	s_rand_digits;
INLINE	void	rand_max()
{
	M_uint64 randmax = ((M_uint64)RAND_MAX <= 0xffffffff) ? RAND_MAX + 1 : 0;
	s_rand_digits = -1;

	while(s_rand_digits++ < 32 && !(randmax & 0x01))
	{
		randmax >>= 1;
	}

	if(!s_rand_digits)
	{
		s_rand_quotient = 1;
		s_rand_remainder = 0;
	}
	else
	{
		s_rand_quotient = 32 / s_rand_digits;
		s_rand_remainder = 32 % s_rand_digits;
	}
}
INLINE	M_uint32	rand_int()
{
	M_sint32 i;
	M_uint32 r = 0;
	M_uint32 mask;
	if(!s_rand_quotient)
		rand_max();

	for(i=0; i<s_rand_quotient; i++)
	{
		r |= rand();
		r <<= s_rand_digits;
	}

	if(s_rand_remainder)
	{
		 mask = (1 << s_rand_remainder) - 1;
		 r |= rand() & mask;
	}

	return r;
}


INLINE	void	 M_b8_init(M_bulletin8* b8)
{
	b8->board = 0;
}
INLINE	void	 M_b8_destroy(M_bulletin8* b8)
{
	b8->board = 0;
}
INLINE	void	M_b8_set(M_bulletin8* b8, M_uint32 mask)
{
	b8->board |= (M_uint8)mask;
}
INLINE	void M_b8_clear(M_bulletin8* b8, M_uint32 mask)
{
	b8->board &= ~(M_uint8)mask;
}
INLINE	M_uint32 M_b8_get(M_bulletin8* b8, M_uint32 mask)
{
	return (M_uint32)(b8->board & (M_uint8)mask);
}
