/* 
	M_list.c : 实现链表结构

	kevin.zheng@gmail.com
	2012/09/01
*/

#include "../MBase_priv.h"
#include "MBase.h"
#include <assert.h>

/*
	initialized single link list
*/
INLINE void slist_init(M_slist* head)
{
	head->next = head;
}
/*
	insert node after head. actually, "head" can be arbitary node in list rather than real head of the link list
	this feature is similar to many other functions in this file
*/
INLINE void slist_insert(M_slist* head, M_slist* node)
{
	node->next = head->next;
	head->next = node;
}
/*
	remove node just after head, "head" here is real head of the link list
*/
INLINE M_slist* slist_remove(M_slist* head)
{
	M_slist* remv = head->next;
	
	if(remv != head)
		head->next = remv->next;
	else
		remv = NULL;

	return remv;
}

INLINE void slist_travel(M_slist* head, traveller_t slist_traveller, void* param)
{
	M_slist* tmp1 = head->next;
	M_slist* tmp2;

	while(tmp1 != head)
	{
		tmp2 = tmp1->next;
		slist_traveller(tmp1, param);
		tmp1 = tmp2;
	}
}

/*
	reverse single list, return new head
*/
INLINE M_slist* slist_reverse_NULL(M_slist* head)
{
	M_slist *cursor, *tmp;

	if(!head || !head->next)
		return head;

	cursor = head->next;
	head->next = NULL;
	while(cursor)
	{
		tmp = cursor->next;
		cursor->next = head;
		head = cursor;
		cursor = tmp;
	}

	return head;
}

INLINE void slist_reverse(M_slist* head)
{
	M_slist *cursor = head->next, *tmp1, *tmp2 = head;
	while(cursor != head)
	{
		tmp1 = cursor->next;
		cursor->next = tmp2;
		tmp2 = cursor;
		cursor = tmp1;
	}

	//acturally, cursor is head now
	cursor->next = tmp2;
}

INLINE M_slist* slist_reverse_count(M_slist* head, M_sint32* count)
{
	M_slist *cursor, *tmp;
	*count = 0;

	if(!head)
		return head;

	*count = 1;
	cursor = head->next;
	head->next = NULL;

	while(cursor)
	{
		tmp = cursor->next;
		cursor->next = head;
		head = cursor;
		cursor = tmp;
		++(*count);
	}

	return head;
}

INLINE M_sint32 slist_empty(M_slist* head)
{
	return (head == head->next);
}


INLINE void dlist_init(M_dlist* head)
{
	head->next = head;
	head->prev = head;
}
/*
	insert node after head, head can be arbitary node
*/
INLINE void dlist_insert(M_dlist* head, M_dlist* node)
{
	node->next = head->next;
	node->prev = head;
	
	head->next = node;
	
	node->next->prev = node;
}
/*
	insert node before head, head can be arbitary node
*/
INLINE void dlist_append(M_dlist* head, M_dlist* node)
{
	node->prev = head->prev;
	node->next = head;

	head->prev = node;
	node->prev->next = node;
}
/*
	remove node just after head
*/
INLINE M_dlist* dlist_remove(M_dlist* head, M_dlist* node)
{

	if(node != head)
	{
		node->next->prev = node->prev;
		node->prev->next = node->next;
		return node;
	}
	else
		return NULL;
}

INLINE void dlist_remove_list(M_dlist* head, M_dlist* start, M_dlist* end)
{
	if(head != start && head != end)
	{
		start->prev->next = end->next;
		end->next->prev = start->prev;
		end->next = NULL;
		start->prev = NULL;
	}
}

INLINE void dlist_insert_list(M_dlist* head, M_dlist* start, M_dlist* end)
{
	if(head != start && head != end)
	{
		end->next = head->next;
		head->next->prev = end;
		head->next = start;
		start->prev = head;
	}
}
INLINE void dlist_append_list(M_dlist* head, M_dlist* start, M_dlist* end)
{
	if(head != start && head != end)
	{
		start->prev = head->prev;
		head->prev->next = start;
		end->next = head;
		head->prev = end;
	}
}

INLINE M_sint32 dlist_empty(M_dlist* head)
{
	return (head == head->next);
}

