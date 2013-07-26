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

#define	M_max(a, b)	((a) > (b) ? (a) : (b))
#define	M_min(a, b)	((a) < (b) ? (a) : (b))


/*
	gnu版本的snprintf最多写size个字符到str中，包括结尾0
	ms版本的_snprintf最多写size个字符到str中，不包括结尾0
	统一后的M_snprintf兼容gnu
*/
MBASE_API	INLINE	void M_snprintf(M_sint8* str, M_size_t size, const M_sint8* format, ...);

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

typedef struct st_bulletin8
{
	M_uint8	board;
} M_bulletin8;

/*
	init/destroy: mainly for multithread scenario
*/
MBASE_API	INLINE	void	M_b8_init(M_bulletin8* b8);
MBASE_API	INLINE	void	M_b8_destroy(M_bulletin8* b8);
MBASE_API	INLINE	void	M_b8_set(M_bulletin8* b8, M_uint32 mask);
MBASE_API	INLINE	void	M_b8_clear(M_bulletin8* b8, M_uint32 mask);
MBASE_API	INLINE	M_uint32 M_b8_get(M_bulletin8* b8, M_uint32 mask);


#ifdef __cplusplus
}
#endif

#endif	//__M_UTILITY_H__