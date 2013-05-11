/* 
	M_data_exchanger.h : 定义多线程数据交换器。

	M_data_exchanger is a multi-writer, multi-reader data structure
	base data structure of M_data_exchanger is M_slist

	M_data_exchanger supports list insert, list remove, node insert
	M_data_exchanger doesn't keep order of nodes, neither FIFO nor LIFO

	kevin.zheng@gmail.com
	2012/09/23

*/
#ifndef __M_DATA_EXCHANGER_H__
#define __M_DATA_EXCHANGER_H__

#ifndef __M_CFG_ATOMIC
#include "pthread.h"
#endif

#include "M_types.h"

#ifdef __cplusplus
	extern "C" {
#endif
#define __M_CFG_COUNT_LQUEUE

#ifdef __M_CFG_ATOMIC
#define data_exchanger_DECLARE\
	M_slist *head
#else
#define data_exchanger_DECLARE\
	M_slist *head;\
	pthread_mutex_t mutex
#endif
/*
	inserted node goes to head list
	removed node comes from tail list
*/
typedef struct st_data_exchanger
{
	data_exchanger_DECLARE;
} M_data_exchanger;


/*
	de_destroy success when queue is empty, returns 0;
	otherwise returns -1

	de_init and de_destroy are both single-thread functions,
	i.e. they are not race safe, can't be called concurrently
*/
MBASE_API INLINE void		de_init(M_data_exchanger* de);
MBASE_API INLINE M_sint32	de_destroy(M_data_exchanger* de);

MBASE_API INLINE M_sint32	de_isempty(M_data_exchanger* de);

/*
	de_insert: insert list to de
	de_remove: get list from de
*/
MBASE_API INLINE void		de_insert(M_data_exchanger* de, M_slist* node);
MBASE_API INLINE void		de_insert_list(M_data_exchanger* de, M_slist* head, M_slist* tail);
MBASE_API INLINE M_slist*	de_remove(M_data_exchanger* de);

#ifdef __cplusplus
}
#endif

#endif	//__M_DATA_EXCHANGER_H__
