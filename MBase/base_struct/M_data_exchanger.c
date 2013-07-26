/* 
	M_data_exchanger.c : 实现多线程数据交换器。

	M_data_exchanger is a multi-writer, multi-reader data structure
	base data structure of M_data_exchanger is M_slist

	M_data_exchanger supports list insert, list remove, node insert
	M_data_exchanger doesn't keep order of nodes, neither FIFO nor LIFO

	kevin.zheng@gmail.com
	2012/09/23
*/

#include "../MBase_priv.h"
#include "MBase.h"

INLINE void de_init(M_data_exchanger* de)
{
	de->head = NULL;

#ifndef __M_CFG_ATOMIC
	pthread_mutex_init(&de->mutex, NULL);
#endif
}

INLINE M_sint32 de_destroy(M_data_exchanger* de)
{
	if(!de_isempty(de))
		return -1;

#ifndef __M_CFG_ATOMIC
	pthread_mutex_destroy(&de->mutex);
#endif

	return 0;
}


INLINE M_sint32 de_isempty(M_data_exchanger* de)
{
	return !de->head;
}

INLINE void de_insert(M_data_exchanger* de, M_slist* node)
{
#ifdef __M_CFG_ATOMIC

	M_slist* tmp = de->head;
	M_slist* oldv;
	do {
		oldv = tmp;
		node->next = tmp;
	} while( (tmp = (M_slist*)atomic_cmpxchg((M_atomic*)&de->head, (M_sintptr)oldv, (M_sintptr)node)) != oldv);

	// 2012-10-7: 下面的写法会出错，不知道为什么，为de->head加上volatile修饰也不行，这个错误折腾了有半个月，郁闷ing...
	//do {
		//node->next = de->head;
	//} while( atomic_cmpxchg((M_atomic*)&(de->head), (M_sintptr)node->next, (M_sintptr)node) != (M_sintptr)node->next);

#else
	pthread_mutex_lock(&de->mutex);

	node->next = de->head;
	de->head = node;

	pthread_mutex_unlock(&de->mutex);
#endif
}

INLINE void		de_insert_list(M_data_exchanger* de, M_slist* head, M_slist* tail)
{
#ifdef __M_CFG_ATOMIC
	
	M_slist* tmp = de->head;
	M_slist* oldv;
	do {
		oldv = tmp;
		tail->next = tmp;
	} while( (tmp = (M_slist*)atomic_cmpxchg((M_atomic*)&de->head, (M_sintptr)oldv, (M_sintptr)head)) != oldv);

	//do {
	//	tail->next = de->head;
	//} while( atomic_cmpxchg((M_atomic*)&(de->head), (M_sintptr)tail->next, (M_sintptr)head) != (M_sintptr)tail->next);

#else
	pthread_mutex_lock(&de->mutex);

	tail->next = de->head;
	de->head = head;

	pthread_mutex_unlock(&de->mutex);
#endif
}

INLINE M_slist* de_remove(M_data_exchanger* de)
{
	M_slist* ret = NULL;

	if(de_isempty(de))
		return NULL;

#ifdef __M_CFG_ATOMIC
	ret = (M_slist*)atomic_xchg_return((M_atomic*)(&(de->head)), (M_sintptr)ret);	
#else

	pthread_mutex_lock(&de->mutex);

	M_swapptr((M_sintptr*)&de->head, (M_sintptr*)&ret);

	pthread_mutex_unlock(&de->mutex);

#endif
	
	return ret;
}
