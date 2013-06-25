/* 
	M_poolinf.c : 定义一个线程安全的内存池界面，只支持固定大小的内存分配。当内存池不是线程安全时，
				  poolinf可以提供一个高效的多线程内存池环境

	kevin.zheng@gmail.com
	2013/05/20
*/

#include "../MBase_priv.h"
#include "MBase.h"
#include <assert.h>

#ifdef __M_CFG_POOL_LEAK
static M_sint32 s_alloc_counts = 0;
INLINE M_sint32	pi_get_mem_cts()
{
	return s_alloc_counts;
}
#endif

INLINE void	pi_init(M_poolinf* pi, M_sint32 max_nr_blocks)
{
	slist_init(&pi->head);
	pi->max_nr_blocks = max_nr_blocks;
	pi->nr_blocks = 0;
	pi->pool = NULL;
	pi->fp_alloc = M_malloc;
	pi->fp_free = M_free;
}

INLINE void	pi_attach(M_poolinf* pi, void* pool, M_malloc_t fp_alloc, M_free_t fp_free)
{
	pi->pool = pool;
	if(pool)
	{
		pi->fp_alloc = fp_alloc;
		pi->fp_free = fp_free;
	}
	else
	{
		pi->fp_alloc = M_malloc;
		pi->fp_free = M_free;
	}
}

static INLINE void pi_freenode(void* node, M_poolinf* pi)
{
	pi->fp_free(node, pi->pool);
}

INLINE void	pi_destroy(M_poolinf* pi)
{
	slist_travel(&pi->head, pi_freenode, pi);
	slist_init(&pi->head);
	pi->nr_blocks = 0;
}

INLINE void*	pi_alloc(M_sint32 size, M_poolinf* pi)
{
#ifdef __M_CFG_POOL_LEAK
	s_alloc_counts++;
#endif
	if(!slist_empty(&pi->head))
	{
		--pi->nr_blocks;
		return slist_remove(&pi->head);
	}
	else
		return pi->fp_alloc(size, pi->pool);
}

INLINE void	pi_free(void* mem, M_poolinf* pi)
{
#ifdef __M_CFG_POOL_LEAK
	s_alloc_counts--;
#endif
	if(pi->nr_blocks < pi->max_nr_blocks)
	{
		slist_insert(&pi->head, (M_slist*)mem);
		++pi->nr_blocks;
	}
	else
		pi->fp_free(mem, pi->pool);
}
