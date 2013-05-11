/* 
	M_lqueue.h : 定义无锁队列。lqueue：lockless queue

	M_lqueue is a multi-writer, single-reader queue, 
	i.e. lqueue_remove or lqueue_remove_list is not thread safe, can't be called concurrently

	kevin.zheng@gmail.com
	2012/09/09
*/
#ifndef __M_LQUEUE_H__
#define __M_LQUEUE_H__

#ifndef __M_CFG_ATOMIC
#include "pthread.h"
#endif

#include "M_types.h"
#include "M_data_exchanger.h"

#ifdef __cplusplus
	extern "C" {
#endif
#define __M_CFG_COUNT_LQUEUE
/*
	inserted node goes to head list
	removed node comes from tail list
*/
typedef struct st_lqueue
{
	data_exchanger_DECLARE;

	M_slist* tail;

#ifdef __M_CFG_COUNT_LQUEUE
	M_sintptr	count;
#endif

} M_lqueue;


/*
	lqueue_destroy success when queue is empty, returns 0;
	otherwise returns -1

	lqueue_init and lqueue_destroy are both single-thread functions,
	i.e. they are not race safe, can't be called concurrently
*/
MBASE_API INLINE void		lqueue_init(M_lqueue* lq);
MBASE_API INLINE M_sint32	lqueue_destroy(M_lqueue* lq);

/*
	if __M_CFG_COUNT_LQUEUE is enabled, lqueue_getcount returns actual nodes count in queue;
	otherwise it returns 1 if queue is not empty, 0 if queue is empty.
*/
MBASE_API INLINE M_sint32   lqueue_getcount(M_lqueue* lq);

/*
	why we don't provide lqueue_insert_list?
	reason is hard to determine the order of nodes in list
	which one in list should be first in? and which one should be first out?
*/
MBASE_API INLINE void		lqueue_insert(M_lqueue* lq, M_slist* node);
MBASE_API INLINE M_slist*	lqueue_remove(M_lqueue* lq);

/*
	remove whole "tail list" from lqueue
*/
MBASE_API INLINE M_slist*	lqueue_remove_list(M_lqueue* lq);

/*
	count:	[in] at most "count" nodes will be removed from queue.
			[out] actual nodes count removed from queue
*/
MBASE_API INLINE M_slist*	lqueue_remove_list_bycount(M_lqueue* lq, M_sint32* count);


#ifdef __cplusplus
}
#endif

#endif	//__M_LQUEUE_H__
