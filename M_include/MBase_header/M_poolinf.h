/* 
	M_poolinf.h : 定义一个线程安全的内存池界面，只支持固定大小的内存分配。当内存池不是线程安全时，
				  poolinf可以提供一个高效的多线程内存池环境

	advantage of poolinf: can seamlessly integrated with other pools, for example: malloc/free
	so actually it is a thread-safe pool interface for other pools

	kevin.zheng@gmail.com
	2013/05/20
*/
#ifndef __M_POOLINF_H__
#define __M_POOLINF_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define mem_pool_DECLARE\
	void*		pool;\
	M_malloc_t	fp_alloc;\
	M_free_t	fp_free

typedef struct st_poolinf
{
	mem_pool_DECLARE;
	M_slist		head;
	M_sint32	max_nr_blocks;
	M_sint32	nr_blocks;
	//void*		pool;			//null if malloc/free is used
	//M_malloc_t	fp_alloc;
	//M_free_t	fp_free;
} M_poolinf;

/*
	light pool doesnot manager size of memory blocks, it's user's responsibility to keep the size correct
	also size of a piece of memory block must be greater than sizeof(void*)
*/
MBASE_API INLINE void	pi_init(M_poolinf* pi, M_sint32 max_nr_blocks);
MBASE_API INLINE void	pi_attach(M_poolinf* pi, void* pool, M_malloc_t fp_alloc, M_free_t fp_free);
MBASE_API INLINE void	pi_destroy(M_poolinf* pi);
MBASE_API INLINE void*	pi_alloc(M_sint32 size, M_poolinf* pi);
MBASE_API INLINE void	pi_free(void* mem, M_poolinf* pi);

#ifdef __M_CFG_POOL_LEAK
MBASE_API INLINE M_sint32	pi_get_mem_cts();
#endif


#ifdef __cplusplus
}
#endif

#endif	//__M_POOLINF_H__