/* 
	M_lightpool.h : 定义一个轻量级内存池，只支持固定大小的内存分配

	advantage of lightpool: can seamlessly integrated with malloc/free

	kevin.zheng@gmail.com
	2013/05/20
*/
#ifndef __M_LIGHTPOOL_H__
#define __M_LIGHTPOOL_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_lightpool
{
	M_slist		head;
	M_sint32	max_nr_blocks;
	M_sint32	nr_blocks;
} M_lightpool;

/*
	light pool doesnot manager size of memory blocks, it's user's responsibility to keep the size correct
	also size of a piece of memory block must be greater than sizeof(void*)
*/
MBASE_API INLINE void	lp_init(M_sint32 max_nr_blocks, M_lightpool* lp);
MBASE_API INLINE void	lp_destroy(M_lightpool* lp);
MBASE_API INLINE void*	lp_alloc(M_sint32 size, M_lightpool* lp);
MBASE_API INLINE void	lp_free(void* mem, M_lightpool* lp);

#ifdef __M_CFG_LP_LEAK
MBASE_API INLINE M_sint32	lp_get_mem_cts();
#endif


#ifdef __cplusplus
}
#endif

#endif	//__M_LIGHTPOOL_H__