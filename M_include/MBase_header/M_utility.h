/* 
	M_utility.h : 一组常用函数

	kevin.zheng@gmail.com
	2012/09/23
*/

#ifndef __M_UTILITY_H__
#define __M_UTILITY_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP(a, b)	\
	do {\
		(a) += (b);	(b) = (a)-(b); (a) -= (b);\
	} while(0)

MBASE_API	INLINE	void M_swap(void* s1, void* s2, M_sintptr len);

MBASE_API	INLINE	void M_swap8(M_sint8* s1, M_sint8* s2);
MBASE_API	INLINE	void M_swap16(M_sint16* s1, M_sint16* s2);
MBASE_API	INLINE	void M_swap32(M_sint32* s1, M_sint32* s2);
#ifdef __M_CFG_OS64
MBASE_API	INLINE	void M_swap64(M_sint64* s1, M_sint64* s2);
#endif
MBASE_API	INLINE	void M_swapptr(M_sintptr* s1, M_sintptr* s2);

/*
	generate random number between 0 - 2^32-1
*/
MBASE_API	INLINE	M_uint32	rand_int();

#ifdef __cplusplus
}
#endif

#endif	//__M_UTILITY_H__