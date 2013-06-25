/* 
	M_lqueue.C : 实现无锁队列。lqueue：lockless queue

	kevin.zheng@gmail.com
	2012/09/09
*/

#include "../MBase_priv.h"
#include "MBase.h"

INLINE void lqueue_init(M_lqueue* lq)
{
	de_init((M_data_exchanger*)lq);
	
	lq->tail = NULL;
#ifdef __M_CFG_COUNT_LQUEUE
	lq->count = 0;
#endif
}

INLINE M_sint32 lqueue_destroy(M_lqueue* lq)
{
	return de_destroy((M_data_exchanger*)lq);
}

/*
	if __M_CFG_COUNT_LQUEUE is enabled, lqueue_getcount returns actual nodes count in queue;
	otherwise it returns 1 if queue is not empty, 0 if queue is empty.
*/
INLINE M_sint32 lqueue_getcount(M_lqueue* lq)
{
#ifdef __M_CFG_COUNT_LQUEUE
	return lq->count;
#else
	if(lq->head || lq->tail)
		return 1;
	else
		return 0;
#endif
}

/*
	why we don't provide lqueue_insert_list?
	reason is hard to determine the order of nodes in list
	which one in list should be first in? and which one should be first out?
*/
INLINE void lqueue_insert(M_lqueue* lq, M_slist* node)
{
	de_insert((M_data_exchanger*)lq, node);

#ifdef __M_CFG_COUNT_LQUEUE
#ifdef __M_CFG_ATOMIC
	atomic_inc((M_atomic*)&lq->count);
#else
	pthread_mutex_lock(&lq->mutex);
	++lq->count;
	pthread_mutex_unlock(&lq->mutex);
#endif
#endif

}
INLINE M_slist* lqueue_remove(M_lqueue* lq)
{
	M_slist* ret = NULL;

	if(!lqueue_getcount(lq))
		return NULL;

#ifdef __M_CFG_ATOMIC

#ifdef __M_CFG_COUNT_LQUEUE
	atomic_dec((M_atomic*)&(lq->count));
#endif

	if(!lq->tail)
	{
		atomic_xchg((M_atomic*)(&(lq->head)), (M_sintptr*)&(lq->tail));	
		lq->tail = slist_reverse_NULL(lq->tail);
	}
#else

	pthread_mutex_lock(&lq->mutex);

#ifdef __M_CFG_COUNT_LQUEUE
	--lq->count;
#endif

	if(!lq->tail)
	{
		ret = (void*)1;
		M_swapptr((M_sintptr*)&lq->head, (M_sintptr*)&lq->tail);
	}

	pthread_mutex_unlock(&lq->mutex);

	if( ret == (void*)1 )
		lq->tail = slist_reverse_NULL(lq->tail);

#endif

	ret = lq->tail;
	lq->tail = ret->next;
	ret->next = NULL;
	
	return ret;
}

INLINE M_slist*	lqueue_remove_list(M_lqueue* lq)
{
	M_slist* ret;
#ifdef __M_CFG_COUNT_LQUEUE
	M_sint32 count;
#endif

	if(!lq->tail)
	{
		lq->tail = de_remove((M_data_exchanger*)lq);

#ifdef __M_CFG_COUNT_LQUEUE

		lq->tail = slist_reverse_count(lq->tail, &count);

#ifdef __M_CFG_ATOMIC
		atomic_sub((M_atomic*)&(lq->count), (M_sintptr)count);
#else
		pthread_mutex_lock(&lq->mutex);
		lq->count -= count;
		pthread_mutex_unlock(&lq->mutex);
#endif

#else
		lq->tail = slist_reverse_NULL(lq->tail);
#endif
	}

	ret = lq->tail;
	lq->tail = NULL;
	return ret;

}

INLINE M_slist* lqueue_remove_list_bycount(M_lqueue* lq, M_sint32* count)
{
	M_slist* ret = NULL;
	M_sint32 i = 0;
	M_slist* tmp;

	if(!lqueue_getcount(lq))
		return NULL;

#ifdef __M_CFG_ATOMIC

	if(!lq->tail)
	{
		atomic_xchg((M_atomic*)(&(lq->head)), (M_sintptr*)&(lq->tail));	
		lq->tail = slist_reverse_NULL(lq->tail);
	}
#else

	pthread_mutex_lock(&lq->mutex);

	if(!lq->tail)
	{
		i = 1;
		M_swapptr((M_sintptr*)&lq->head, (M_sintptr*)&lq->tail);
	}

	pthread_mutex_unlock(&lq->mutex);

	if( i == 1)
		lq->tail = slist_reverse_NULL(lq->tail);

#endif

	ret = lq->tail;
	tmp = ret;
	i = 1;
	while(i < *count && tmp)
	{
		++i;
		tmp = tmp->next;
	}

	if(tmp)
	{
		lq->tail = tmp->next;
		tmp->next = NULL;
		*count = i;
	}
	else
	{
		lq->tail = NULL;
		*count = i - 1;
	}

#ifdef __M_CFG_COUNT_LQUEUE

#ifdef __M_CFG_ATOMIC
	atomic_sub((M_atomic*)&(lq->count), *count);
#else
	pthread_mutex_lock(&lq->mutex);
	lq->count -= *count;
	pthread_mutex_unlock(&lq->mutex);
#endif
	
#endif

	return ret;
}
