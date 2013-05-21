/* 
	M_list.h : 定义链表结构

	kevin.zheng@gmail.com
	2012/09/01
*/
#ifndef __M_LIST_H__
#define __M_LIST_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_slist
{
	struct st_slist* next;
} M_slist;

typedef struct st_dlist
{
	struct st_dlist *prev, *next;
} M_dlist;

/*
	initialized single link list
*/
MBASE_API INLINE void slist_init(M_slist* head);
/*
	insert node after head. actually, "head" can be arbitary node in list rather than real head of the link list
	this feature is similar to many other functions in this file
*/
MBASE_API INLINE void slist_insert(M_slist* head, M_slist* node);
/*
	remove node just after head, "head" here is real head of the link list
	return removed node
*/
MBASE_API INLINE M_slist* slist_remove(M_slist* head);

MBASE_API INLINE void slist_travel(M_slist* head, traveller_t slist_traveller, void* param);


/*
	reverse single list, return new head

	BE AWARE: list here is a normal list, end with NULL pointer, not a loop list
*/
MBASE_API INLINE M_slist* slist_reverse(M_slist* head);

/*
	count nodes when reversing list
*/
MBASE_API INLINE M_slist* slist_reverse_count(M_slist* head, M_sint32* count);

/*
	return 1 if slist is empty, 0 otherwise
*/
MBASE_API INLINE M_sint32 slist_empty(M_slist* head);

///*
//	APIs for iterator
//
//	getnext: get next node, trival. if next node is head, return head
//	getprev: get prev node, need loop. if prev node is head, return head
//	insert_next: insert "stub" next of "node"
//	insert_prev: insert "stub" prev of "node"
//	remove_next; remove next node if exists(next node is not head), trival
//	remove_self: remove self, call getprev first. NOTE: to be remove node MUST NOT be head
//	remove_prev: remove prev node if exists(prev node is not head), need double loop
//
//	all double pointers in insert/remove functions are for consistency of rbtree
//
//	ishead: check if node is head of list, return 1 if node is head of list, 0 otherwise
//	istail: check if node is tail of list, return 1 if node is tail of list, 0 otherwise
//*/
//MBASE_API INLINE M_slist* slist_getnext(M_slist* head, M_slist* node);
//MBASE_API INLINE M_slist* slist_getprev(M_slist* head, M_slist* node);
//MBASE_API INLINE void slist_insert_next(M_slist** head, M_slist* node, M_slist* stub);
//MBASE_API INLINE void slist_insert_prev(M_slist** head, M_slist* node, M_slist* stub);
//MBASE_API INLINE M_slist* slist_remove_next(M_slist** head, M_slist* node);
//MBASE_API INLINE M_slist* slist_remove_self(M_slist** head, M_slist* node);
//MBASE_API INLINE M_slist* slist_remove_prev(M_slist** head, M_slist* node);
//MBASE_API INLINE M_sint32 slist_ishead(M_slist* head, M_slist* node);
//MBASE_API INLINE M_sint32 slist_istail(M_slist* head, M_slist* node);

MBASE_API INLINE void dlist_init(M_dlist* head);
/*
	insert node after head, head can be arbitary node
*/
MBASE_API INLINE void dlist_insert(M_dlist* head, M_dlist* node);
/*
	insert node before head, head can be arbitary node
*/
MBASE_API INLINE void dlist_append(M_dlist* head, M_dlist* node);
/*
	remove node just after head, "head" here is real head of the link list
	return removed node
*/
MBASE_API INLINE M_dlist* dlist_remove(M_dlist* head);
/*
	return 1 if dlist is empty, 0 otherwise
*/
MBASE_API INLINE M_sint32 dlist_empty(M_dlist* head);


#ifdef __cplusplus
}
#endif

#endif	//__M_LIST_H__