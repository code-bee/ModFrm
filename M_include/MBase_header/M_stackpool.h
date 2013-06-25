/* 
	M_stackpool.h : 定义一个轻量级内存池，可支持任意大小的内存分配
					适用于许多小内存可同时释放的场景

	kevin.zheng@gmail.com
	2013/06/04
*/
#ifndef __M_STACKPOOL_H__
#define __M_STACKPOOL_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_stackpool
{
	M_sint8*	pool;			//base address of memory block
	M_sint8*	cur_ptr;		//current available address
	M_sint8*	last_ptr;		//address of last alloced memory
	M_sint32	pool_size;		//size of pool
#ifdef _DEBUG
	M_sint8*	max_ptr;		//max pointer position
#endif
} M_stackpool;

/*
	if an application needs many small size memory pieces in a short time, 
	or if destroy time of these memory pieces are exactly simultaneous,
	stack pool is a good way to avoid memory leak

	for implementation simplicity, we don't care about 4 char's alignment
*/
MBASE_API INLINE void	sp_init(void* mem, M_sint32 mem_size, M_stackpool* sp);
MBASE_API INLINE void	sp_destroy(M_stackpool* sp);
MBASE_API INLINE void*	sp_alloc(M_sint32 size, M_stackpool* sp);
MBASE_API INLINE void	sp_free(void* mem, M_stackpool* sp);

/*
	hwm: high water mark. this API only works well if _DEBUG is on
*/
MBASE_API INLINE M_sint32	sp_hwm(M_stackpool* sp);

#ifdef __cplusplus
}
#endif

#endif	//__M_STACKPOOL_H__