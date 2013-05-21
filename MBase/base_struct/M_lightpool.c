/* 
	M_lightpool.c : 定义一个轻量级内存池，只支持固定大小的内存分配

	kevin.zheng@gmail.com
	2013/05/20
*/

#include "../MBase_priv.h"
#include "MBase.h"
#include <assert.h>

INLINE void	lp_init(M_sint32 max_nr_blocks, M_lightpool* lp)
{
	slist_init(&lp->head);
	lp->max_nr_blocks = max_nr_blocks;
	lp->nr_blocks = 0;
}

void lp_freenode(void* node, void* param)
{
	free(node);
}

INLINE void	lp_destroy(M_lightpool* lp)
{
	slist_travel(&lp->head, lp_freenode, NULL);
	slist_init(&lp->head);
	lp->nr_blocks = 0;
}

INLINE void*	lp_alloc(M_sint32 size, M_lightpool* lp)
{
	if(!slist_empty(&lp->head))
	{
		--lp->nr_blocks;
		return slist_remove(&lp->head);
	}
	else
		return malloc(size);
}

INLINE void	lp_free(void* mem, M_lightpool* lp)
{
	if(lp->nr_blocks < lp->max_nr_blocks)
	{
		slist_insert(&lp->head, (M_slist*)mem);
		++lp->nr_blocks;
	}
	else
		free(mem);
}
