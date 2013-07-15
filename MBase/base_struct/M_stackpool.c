/* 
	M_stackpool.c : 定义一个轻量级内存池，可支持任意大小的内存分配

	kevin.zheng@gmail.com
	2013/06/04
*/

#include "../MBase_priv.h"
#include "MBase.h"
#include <assert.h>


INLINE void	sp_init(void* mem, M_sint32 mem_size, M_stackpool* sp)
{
	sp->pool = (M_sint8*)mem;
	sp->cur_ptr = (M_sint8*)mem;
	sp->last_ptr = (M_sint8*)NULL;
	sp->pool_size = mem_size;
#ifdef __M_CFG_POOL_LEAK
	memset(mem, 0, mem_size);
#endif
#ifdef _DEBUG
	sp->max_ptr = sp->cur_ptr;
#endif
}

INLINE void	sp_destroy(M_stackpool* sp)
{
#ifdef __M_CFG_POOL_LEAK
	M_sint8* tmp;
	M_sint32* tmp_bak;
	tmp = sp->pool;
	while(tmp < sp->pool + sp->pool_size)
	{
		while(tmp < sp->pool + sp->pool_size && !*tmp){++tmp;};

		tmp_bak = (M_sint32*)tmp;
		while(tmp < sp->pool + sp->pool_size && *tmp){++tmp;};

		printf("memory leak at 0x%p\n", tmp_bak);
		while(tmp_bak < tmp)
			printf("\t0x%p: %x\t%x\t%x\t%x\n", tmp_bak, *tmp_bak++, *tmp_bak++, *tmp_bak++, *tmp_bak++);
	}
#endif
}

INLINE void* sp_alloc(M_sint32 size, M_stackpool* sp)
{
#ifdef __M_CFG_POOL_LEAK
	M_sint8* tmp = sp->cur_ptr;
	if(sp->cur_ptr + size + sizeof(M_sint32) <= sp->pool + sp->pool_size)
	{
		sp->cur_ptr += size + sizeof(M_sint32);
		sp->last_ptr = tmp;
		*(M_sint32*)tmp = size;
#ifdef _DEBUG
		if(sp->cur_ptr > sp->max_ptr)
			sp->max_ptr = sp->cur_ptr;
#endif
		memset(tmp + sizeof(M_sint32), 1, size);
		return tmp + sizeof(M_sint32);
	}
	else
		return NULL;
#else
	M_sint8* tmp = sp->cur_ptr;
	if(sp->cur_ptr + size <= sp->pool + sp->pool_size)
	{
		sp->cur_ptr += size;
		sp->last_ptr = tmp;
#ifdef _DEBUG
		if(sp->cur_ptr > sp->max_ptr)
			sp->max_ptr = sp->cur_ptr;
#endif
		return tmp;
	}
	else
		return NULL;
#endif
}

INLINE void	sp_free(void* mem, M_stackpool* sp)
{
#ifdef __M_CFG_POOL_LEAK
	M_sint8* m = (M_sint8*)mem - sizeof(M_sint32);
	M_sint32 size = *(M_sint32*)m;
	memset(m, size + sizeof(M_sint32), 0);
	if(sp->last_ptr == m)
	{
		sp->cur_ptr = sp->last_ptr;
		sp->last_ptr = NULL;
	}
#else
	if(sp->last_ptr == mem)
	{
		sp->cur_ptr = sp->last_ptr;
		sp->last_ptr = NULL;
	}
#endif
}


INLINE M_sint32	sp_hwm(M_stackpool* sp)
{
#ifdef _DEBUG
	return sp->max_ptr - sp->pool;
#else
	return -1;
#endif
}
