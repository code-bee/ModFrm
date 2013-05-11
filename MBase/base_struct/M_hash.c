/* 
	M_hash.c : 实现hash结构

	kevin.zheng@gmail.com
	2012/09/09
*/

#include <assert.h>

#include "../MBase_priv.h"
#include "MBase.h"

/*
	typedefs for hash iterator
	M_sint8: general type of slot in hash table, actually they are all pointers in hash table
	get_nextprev_f_t: get next/prev node of "stub" in slist/dlist/rbtree "head"
	insert_f_t: insert "stub" next/prev of "node" in slist/dlist/rbtree "head"
	remove_f_t: remove and return next/prev of "node" in slist/dlist/rbtree "head"
	is_headtail_f_t: test if "stub" is head/tail of slist/dlist/rbtree "head"
*/
typedef M_sint32 (*isemtpy_f_t)(void* head);
typedef void* (*get_nextprev_f_t)(void* head, void* stub);
typedef void (*insert_f_t)(void** head, void* node, void* stub, void* hash);
typedef void* (*remove_f_t)(void** head, void* node, void* hash);
typedef M_sint32 (*is_headtail_f_t)(void* head, void* stub);

/*
	support APIs for iterator

	getnext: get next node, trival. if next node is head, return NULL
	getprev: get prev node, trival. if prev node is head, return NULL
	insert_next: insert "stub" next of "node"
	insert_prev: insert "stub" prev of "node"
	remove_next; remove next node if exists(next node is not head), trival
	remove_self: remove self, trival. NOTE: to be remove node MUST NOT be head
	remove_prev: remove prev node if exists(prev node is not head), trival

	all double pointers in insert/remove functions are for consistency of rbtree

	ishead: check if node is head of list, return 1 if node is head of list, 0 otherwise
	istail: check if node is tail of list, return 1 if node is tail of list, 0 otherwise
*/


INLINE M_slist* slist_getnext(M_slist* head, M_slist* node)
{
	//return node->next == head ? NULL : node->next;
	head = head;
	return node->next;
}

INLINE M_slist* _slist_getprev(M_slist* head, M_slist* node)
{
	M_slist* tmp = head;
#ifdef _DEBUG
	M_sint32 i = 0;
#endif
	
	while(tmp->next != node)
	{
		tmp = tmp->next;
		
		//detect dead loop, because if node is not in the list, dead loop is definitely happen
#ifdef _DEBUG
		if( tmp == head && (++i > 1) )
			assert(0);
#endif
	}
	
	return tmp;
}

INLINE M_slist* slist_getprev(M_slist* head, M_slist* node)
{
	M_slist* prev = _slist_getprev(head, node);
	//return prev == head ? NULL : prev;
	return prev;
}

INLINE void slist_insert_next(M_slist** head, M_slist* node, M_slist* stub, void* hash)
{
	head = head;
	hash = hash;
	slist_insert(node, stub);
}
INLINE void slist_insert_prev(M_slist** head, M_slist* node, M_slist* stub, void* hash)
{
	M_slist* prev = slist_getprev(*head, node);
	hash = hash;
	slist_insert(prev, stub);
}

/*
	remove node after "node"
*/
INLINE M_slist* slist_remove_next(M_slist** head, M_slist* node, void* hash)
{
	M_slist* remv = node->next;
	hash = hash;
	
	if(remv != *head)
		node->next = remv->next;
	else
		remv = NULL;

	return remv;
}

INLINE M_slist* slist_remove_self(M_slist** head, M_slist* node, void* hash)
{
	M_slist* prev = _slist_getprev(*head, node);
	hash = hash;

	prev->next = node->next;
	return node;
}

INLINE M_slist* slist_remove_prev(M_slist** head, M_slist* node, void* hash)
{
	M_slist* remv = _slist_getprev(*head, node);
	hash = hash;

	if(remv != *head)
		return slist_remove_self(head, remv, NULL);
	else
		return NULL;
}

INLINE M_sint32 slist_ishead(M_slist* head, M_slist* node)
{
	return (head->next == node);
}
INLINE M_sint32 slist_istail(M_slist* head, M_slist* node)
{
	return (node->next == head);
}

INLINE M_dlist* dlist_getnext(M_dlist* head, M_dlist* node)
{
	//return node->next == head ? NULL : node->next;
	head = head;
	return node->next;
}
INLINE M_dlist* dlist_getprev(M_dlist* head, M_dlist* node)
{
	//return node->prev == head ? NULL : node->prev;
	head = head;
	return node->prev;
}

INLINE void dlist_insert_next(M_dlist** head, M_dlist* node, M_dlist* stub, void* hash)
{
	head = head;
	hash = hash;
	dlist_insert(node, stub);
}
INLINE void dlist_insert_prev(M_dlist** head, M_dlist* node, M_dlist* stub, void* hash)
{
	head = head;
	hash = hash;
	dlist_append(node, stub);
}

INLINE M_dlist* dlist_remove_next(M_dlist** head, M_dlist* node, void* hash)
{
	M_dlist* remv = node->next;
	hash = hash;

	if(remv != *head)
	{
		node->next = remv->next;
		remv->next->prev = node;
	}
	else
		remv = NULL;

	return remv;
}

INLINE M_dlist* dlist_remove_self(M_dlist** head, M_dlist* node, void* hash)
{
	head = head;
	hash = hash;

	node->prev->next = node->next;
	node->next->prev = node->prev;

	return node;
}

INLINE M_dlist* dlist_remove_prev(M_dlist** head, M_dlist* node, void* hash)
{
	M_dlist* remv = node->prev;
	hash = hash;

	if(remv != *head)
	{
		node->prev = remv->prev;
		remv->prev->next = node;
	}
	else
		remv = NULL;

	return remv;
}

INLINE M_sint32 dlist_ishead(M_dlist* head, M_dlist* node)
{
	return (head->next == node);
}
INLINE M_sint32 dlist_istail(M_dlist* head, M_dlist* node)
{
	return (head->prev == node);
}

INLINE M_sint32 bst_empty(M_bst_stub** root)
{
	return !*root;
}

INLINE M_bst_stub* bst_getnext(M_bst_stub** root, M_bst_stub* stub)
{
	M_bst_stub* ret;
	if( (void*)root == (void*)stub )
		return bst_get_first(*root);
	else
	{
		if( !(ret = bst_successor(stub)) )
			return (M_bst_stub*)root;
		else
			return ret;
	}
}
INLINE M_bst_stub* bst_getprev(M_bst_stub** root, M_bst_stub* stub)
{
	M_bst_stub* ret;
	if( (void*)root == (void*)stub )
		return bst_get_last(*root);
	else
	{
		if( !(ret = bst_predecessor(stub)) )
			return (M_bst_stub*)root;
		else
			return ret;
	}
}
INLINE void _bst_insert(M_bst_stub*** root, M_bst_stub* node, M_bst_stub* stub, M_hash* hash)
{
	node = node;
	rbt_insert(*root, stub, hash->cmp_key, hash->get_key, hash->get_color, hash->set_color);
}
INLINE M_bst_stub* bst_remove_self(M_bst_stub*** root, M_bst_stub* node, M_hash* hash)
{
	rbt_remove_node(*root, node, hash->get_color, hash->set_color);
	return node;
}
INLINE M_bst_stub* bst_remove_next(M_bst_stub*** root, M_bst_stub* node, M_hash* hash)
{
	M_bst_stub* next = bst_successor(node);
	if(next)
		rbt_remove_node(*root, next, hash->get_color, hash->set_color);
	
	return next;
}
INLINE M_bst_stub* bst_remove_prev(M_bst_stub*** root, M_bst_stub* node, M_hash* hash)
{
	M_bst_stub* prev = bst_predecessor(node);
	if(prev)
		rbt_remove_node(*root, prev, hash->get_color, hash->set_color);
	
	return prev;
}
INLINE M_sint32 bst_ishead(M_bst_stub** root, M_bst_stub* node)
{
	return node == bst_get_first(*root);
}
INLINE M_sint32 bst_istail(M_bst_stub** root, M_bst_stub* node)
{
	return node == bst_get_last(*root);
}

/*
	hash object implementation
*/

#define HT_NUM (HT_RBTREE+1)

static INLINE void* hash_slot_addr(M_hash* hash, M_sint32 index)
{
	switch(hash->hash_type)
	{
	case HT_SLIST:
		return ((M_slist*)hash->ht) + index;
	case HT_DLIST:
		return ((M_dlist*)hash->ht) + index;
	case HT_RBTREE:
		return ((M_bst_stub**)hash->ht) + index;
	default:
		return NULL;
	}
}
//#define hash_slot_addr(hash, type, index)	(((type*)(hash)->ht) + (index))

static INLINE void	hash_set_func(M_hash* hash, hash_key_t hash_key, cmp_key_t cmp_key, get_key_t get_key, 
								  get_rbcolor_t get_color, set_rbcolor_t set_color)
{
	hash->hash_key = hash_key;
	hash->cmp_key = cmp_key;
	hash->get_key = get_key;
	hash->get_color = get_color;
	hash->set_color = set_color;
}

static INLINE M_sint32	hash_get_table_size(M_sint32 hash_type, M_size hash_table_len)
{
	//return hash_type * hash_table_len;
	//return sizeof(void*) * hash_table_len;
	switch(hash_type)
	{
	case HT_SLIST:
		return sizeof(M_slist) * hash_table_len;
	case HT_DLIST:
		return sizeof(M_dlist) * hash_table_len;
	case HT_RBTREE:
		return sizeof(M_bst_stub*) * hash_table_len;
	default:
		return 0;
	}
}

static INLINE M_sint32	hash_destroy(M_hash* hash)
{
	hash = hash;
	return 0;
}

static INLINE M_sint32	hash_init(M_hash* hash, void* hash_table, M_size hash_table_len, M_sint32 hash_type);

INLINE void M_hash_construct(M_hash* obj)
{
	M_hash* hash = obj;

	hash->init = hash_init;
	hash->destroy = hash_destroy;
	hash->set_func = hash_set_func;
	hash->get_table_size = hash_get_table_size;
}


typedef void (*list_insert_t)(void* head, void* stub);

static INLINE void	list_hash_insert(M_hash* hash, void* hash_stub, list_insert_t list_insert)
{
	void* key = hash->get_key(hash_stub);
	M_size index = (hash->hash_key(key)) % hash->ht_len;

	list_insert(hash_slot_addr(hash, index), hash_stub);
}

static INLINE M_sint32	list_hash_insearch(M_hash* hash, void* hash_stub, list_insert_t list_insert)
{
	void *key = hash->get_key(hash_stub), *key2;

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_slist* head = hash_slot_addr(hash, index);
	M_slist* cursor = head->next;

	while(cursor != head)
	{
		key2 = hash->get_key(cursor);
		if(!hash->cmp_key(key, key2))
			return 0;

		cursor = cursor->next;
	}

	list_insert(head, hash_stub);
	return 1;
}

static INLINE void*	list_hash_search(M_hash* hash, void* key)
{
	void* key2;

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_slist* head = hash_slot_addr(hash, index);
	M_slist* cursor = head->next;

	while(cursor != head)
	{
		key2 = hash->get_key(cursor);
		if(!hash->cmp_key(key, key2))
			return cursor;

		cursor = cursor->next;
	}

	return NULL;
}

//static M_hash_interface s_hi[HT_NUM];

/*
	slist hash object implementation
*/

static INLINE void	slist_hash_insert(M_hash* hash, M_slist* hash_stub)
{
	list_hash_insert(hash, hash_stub, slist_insert);
}

static INLINE M_sint32	slist_hash_insearch(M_hash* hash, M_slist* hash_stub)
{
	return list_hash_insearch(hash, hash_stub, slist_insert);
}

static INLINE void*	slist_hash_remove_node(M_hash* hash, M_slist* hash_stub)
{
	void *key = hash->get_key(hash_stub);

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_slist* head = hash_slot_addr(hash, index);
	M_slist* cursor = head->next;
	M_slist* prev = head;

	while(cursor != head)
	{
		if(cursor == hash_stub)
		{
			prev->next = cursor->next;
			return cursor;
		}

		prev = cursor;
		cursor = cursor->next;
	}

	return NULL;
}

static INLINE void*	slist_hash_remove(M_hash* hash, M_slist* key)
{
	void *key2;

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_slist* head = hash_slot_addr(hash, index);
	M_slist* cursor = head->next;
	M_slist* prev = head;

	while(cursor != head)
	{
		key2 = hash->get_key(cursor);
		if(!hash->cmp_key(key, key2))
		{
			prev->next = cursor->next;
			return cursor;
		}

		prev = cursor;
		cursor = cursor->next;
	}

	return NULL;
}

/*
	dlist hash object implementation
*/

static INLINE void	dlist_hash_insert(M_hash* hash, M_dlist* hash_stub)
{
	list_hash_insert(hash, hash_stub, dlist_insert);
}

static INLINE M_sint32	dlist_hash_insearch(M_hash* hash, M_dlist* hash_stub)
{
	return list_hash_insearch(hash, hash_stub, dlist_insert);
}

static INLINE void*	dlist_hash_remove_node(M_hash* hash, M_dlist* hash_stub)
{
#ifdef _DEBUG
	void *key = hash->get_key(hash_stub);

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_dlist* head = hash_slot_addr(hash, index);
	
	if(hash_stub == head)
		return NULL;
#endif

	return dlist_remove_self(NULL, hash_stub, NULL);
}

static INLINE void*	dlist_hash_remove(M_hash* hash, M_dlist* key)
{
	void *key2;

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_dlist* head = hash_slot_addr(hash, index);
	M_dlist* cursor = head->next;

	while(cursor != head)
	{
		key2 = hash->get_key(cursor);
		if(!hash->cmp_key(key, key2))
			return dlist_remove_self(NULL, cursor, NULL);

		cursor = cursor->next;
	}

	return NULL;
}

/*
	rbtree hash object implementation
*/

static INLINE void	bst_hash_insert(M_hash* hash, M_bst_stub* hash_stub)
{
	void* key = hash->get_key(hash_stub);
	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_bst_stub** root = hash_slot_addr(hash, index);

	_bst_insert(&root, NULL, hash_stub, hash);
}

static INLINE M_sint32	bst_hash_insearch(M_hash* hash, M_bst_stub* hash_stub)
{
	void *key = hash->get_key(hash_stub);

	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_bst_stub** root = hash_slot_addr(hash, index);

	return rbt_insearch(root, hash_stub, hash->cmp_key, hash->get_key, hash->get_color, hash->set_color);
}

static INLINE void*	bst_hash_search(M_hash* hash, void* key)
{
	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_bst_stub** root = hash_slot_addr(hash, index);

	return bst_search(*root, key, hash->cmp_key, hash->get_key);
}

static INLINE void*	bst_hash_remove_node(M_hash* hash, M_bst_stub* hash_stub)
{
	void *key = hash->get_key(hash_stub);
	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_bst_stub** root = hash_slot_addr(hash, index);

	rbt_remove_node(root, hash_stub, hash->get_color, hash->set_color);
	return hash_stub;
}

static INLINE void*	bst_hash_remove(M_hash* hash, M_bst_stub* key)
{
	M_size index = (hash->hash_key(key)) % hash->ht_len;
	M_bst_stub** root = hash_slot_addr(hash, index);

	return rbt_remove(root, key, hash->cmp_key, hash->get_key, hash->get_color, hash->set_color);
}

static INLINE M_sint32	hash_init(M_hash* hash, void* hash_table, M_size hash_table_len, M_sint32 hash_type)
{
	M_sint32 i = 0;
	if(hash_type >= HT_NUM)
		return -1;

	//hash->i = &s_hi[hash_type];
	hash->ht = hash_table;
	hash->ht_len = hash_table_len;
	hash->hash_type = hash_type;
	//hash->current = NULL;

	switch(hash_type)
	{
	case HT_SLIST:

		hash->insert = slist_hash_insert;
		hash->insearch = slist_hash_insearch;
		hash->search = list_hash_search;
		hash->remove = slist_hash_remove;
		hash->remove_node = slist_hash_remove_node;

		for(i=0; i<hash_table_len; ++i)
			slist_init(hash_slot_addr(hash, i));
			//slist_init(&(((M_slist*)hash_table)[i]));
		break;
	case HT_DLIST:

		hash->insert = dlist_hash_insert;
		hash->insearch = dlist_hash_insearch;
		hash->search = list_hash_search;
		hash->remove = dlist_hash_remove;
		hash->remove_node = dlist_hash_remove_node;

		for(i=0; i<hash_table_len; ++i)
			dlist_init(hash_slot_addr(hash, i));
			//dlist_init(&(((M_dlist*)hash_table)[i]));
		break;
	case HT_RBTREE:
		hash->insert = bst_hash_insert;
		hash->insearch = bst_hash_insearch;
		hash->search = bst_hash_search;
		hash->remove = bst_hash_remove;
		hash->remove_node = bst_hash_remove_node;

		memset(hash_table, 0, sizeof(M_bst_stub*) * hash_table_len);
		break;
	default:
		return -1;
	}

	return 0;
}

/*
	common iterator implementation
*/

static INLINE void* iter_get_current(M_iter* iter)
{
	return iter->current;
}

static INLINE void iter_attach(M_iter* iter, void* container)
{
	iter->container = container;
}

static INLINE void iter_detach(M_iter* iter)
{
	iter->container = NULL;
	iter->current = NULL;
}

INLINE void M_iter_construct(M_iter* obj)
{
	obj->attach = iter_attach;
	obj->detach = iter_detach;
	obj->get_current = iter_get_current;

	obj->container = NULL;
	obj->current = NULL;
}

/*
	hash iterator implementation
*/

#define HI_INVALID			0
#define HI_FIRST_CURSOR		1
#define HI_LAST_CURSOR		2

static INLINE void  hash_iter_attach(M_hash_iter* hash_iter, M_hash* container);
static INLINE void* hash_iter_search(M_hash_iter* hash_iter, void* key);
static INLINE M_sint32 hash_iter_set_bucket(M_hash_iter* hash_iter, M_size bucket_size, M_size bucket_id);

INLINE void M_hash_iter_construct(M_hash_iter* obj)
{
	M_iter_construct((M_iter*)obj);
	obj->attach = hash_iter_attach;
	//obj->detach = iter_detach;
	//obj->get_current = iter_get_current;
	obj->search = hash_iter_search;
	obj->set_bucket = hash_iter_set_bucket;
}

static INLINE M_sint32 hash_iter_set_bucket(M_hash_iter* hash_iter, M_size bucket_size, M_size bucket_id)
{
	M_size hash_len = ((M_hash*)hash_iter->container)->ht_len;

	hash_iter->bucket_begin = bucket_size * bucket_id;
	if(hash_iter->bucket_begin >= hash_len)
	{
		hash_iter->bucket_begin = 0;
		return -1;
	}

	hash_iter->flags = HI_INVALID;

	hash_iter->bucket_end = hash_iter->bucket_begin + bucket_size;
	if(hash_iter->bucket_end > hash_len)
		hash_iter->bucket_end = hash_len;

	hash_iter->bucket_id = bucket_id;
	hash_iter->bucket_size = bucket_size;
	hash_iter->current_index = hash_iter->bucket_begin;

	return 0;
}

static INLINE void* hash_iter_search(M_hash_iter* hash_iter, void* key)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	hash_iter->current = hash->search(hash, key);
	hash_iter->current_index = hash->hash_key(key) % hash->ht_len;
	hash_iter->flags = HI_INVALID;
	return hash_iter->current;
}

/*
	list hash iterator implementation
*/



#define move_next_slot(cur_slot, last_slot, empty_func, hash_iter)\
	do {\
		++(hash_iter->current_index);\
		cur_slot += hash_iter->hash_slot_size;\
	} while(empty_func(cur_slot) && cur_slot < last_slot)
	
//while(empty_func(cur_slot) && (++(hash_iter->current_index), cur_slot++ < last_slot) )

#define move_prev_slot(cur_slot, head_slot, empty_func, hash_iter)\
	do {\
		--(hash_iter->current_index);\
		cur_slot -= hash_iter->hash_slot_size;\
	} while(empty_func(cur_slot) && cur_slot >= head_slot)

//while(empty_func(cur_slot) && (--(hash_iter->current_index), cur_slot-- >= head_slot) )

#define move_next_pos(cur_pos, cur_slot, hash, hash_iter, last_slot, empty_func, next_func/*, tmp*/)\
	do {\
		if(cur_pos && (cur_pos = next_func(cur_slot, cur_pos)) == cur_slot )\
		{\
			/*last_slot = (M_slist*)hash->ht + hash_iter->bucket_end;*/\
			last_slot = hash_slot_addr(hash, hash_iter->bucket_end);\
			move_next_slot(cur_slot, last_slot, empty_func, hash_iter);\
			cur_pos = (cur_slot == last_slot) ? NULL : next_func(cur_slot, cur_slot);\
		}\
	} while(0)
	/*do {\
		if(cur_pos)\
		{\
			if( (tmp = next_func(cur_slot, cur_pos)) != cur_slot)\
				cur_pos = tmp;\
			else\
			{\
				last_slot = (M_slist*)hash->ht + hash_iter->bucket_end;\
				move_next_slot(cur_slot, last_slot, empty_func, hash_iter);\
				cur_pos = (cur_slot == last_slot) ? NULL : next_func(cur_slot);\
			}\
		}\
	} while(0)*/

#define move_prev_pos(cur_pos, cur_slot, hash, hash_iter, head_slot, empty_func, prev_func/*, tmp*/)\
	do {\
		if(cur_pos && (cur_pos = prev_func(cur_slot, cur_pos)) == cur_slot)\
		{\
			/*head_slot = (M_slist*)hash->ht + hash_iter->bucket_begin;*/\
			head_slot = hash_slot_addr(hash, hash_iter->bucket_begin);\
			move_prev_slot(cur_slot, head_slot, empty_func, hash_iter);\
			cur_pos = (cur_slot < head_slot) ? NULL : prev_func(cur_slot, cur_slot);\
		}\
	} while(0)
	/*do {\
		if(cur_pos)\
		{\
			if( (tmp = prev_func(cur_slot, cur_pos)) != cur_slot)\
				cur_pos = tmp;\
			else\
			{\
				head_slot = (M_slist*)hash->ht + hash_iter->bucket_begin;\
				move_prev_slot(cur_slot, head_slot, empty_func, hash_iter);\
				cur_pos = (cur_slot < head_slot) ? NULL : prev_func(cur_slot);\
			}\
		}\
	} while(0)*/

static INLINE void* _hash_iter_begin(M_hash_iter* hash_iter, isemtpy_f_t empty_func, get_nextprev_f_t next_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	M_sint8* head = hash_slot_addr(hash, hash_iter->bucket_begin);//(void**)hash->ht + hash_iter->bucket_begin;
	M_sint8* last;// = hash_slot_addr(hash, hash_iter->bucket_end);//(M_slist*)hash->ht + hash_iter->bucket_end;
	
	hash_iter->current_index = hash_iter->bucket_begin;
	hash_iter->current = head;

	move_next_pos(hash_iter->current, head, hash, hash_iter, last, empty_func, next_func);

	if(hash_iter->current)
		hash_iter->flags = HI_FIRST_CURSOR;

	return hash_iter->current;
}

static INLINE void* _hash_iter_end(M_hash_iter* hash_iter, isemtpy_f_t empty_func, get_nextprev_f_t prev_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	M_sint8* head;// = hash_slot_addr(hash, hash_iter->bucket_begin);//(M_slist*)hash->ht + hash_iter->bucket_begin;
	M_sint8* last = hash_slot_addr(hash, hash_iter->bucket_end-1); //(M_slist*)hash->ht + hash_iter->bucket_end - 1;
	
	hash_iter->current_index = hash_iter->bucket_end - 1;
	hash_iter->current = last;

	move_prev_pos(hash_iter->current, last, hash, hash_iter, head, empty_func, prev_func);

	if(hash_iter->current)
		hash_iter->flags = HI_LAST_CURSOR;
	
	return hash_iter->current;
}

static INLINE void* _hash_iter_move_next(M_hash_iter* hash_iter, isemtpy_f_t empty_func, get_nextprev_f_t next_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	
	void* current = hash_iter->current;
	M_sint8* current_slot = hash_slot_addr(hash, hash_iter->current_index);
	M_sint8* last;

	//move_next_pos(hash_iter->current, current_slot, hash, hash_iter, last, empty_func, next_func);
	if(hash_iter->current && (hash_iter->current = next_func(current_slot, hash_iter->current)) == current_slot )
	{
		/*last_slot = (M_slist*)hash->ht + hash_iter->bucket_end;*/
		last = hash_slot_addr(hash, hash_iter->bucket_end);
		//move_next_slot(current_slot, last, empty_func, hash_iter);
		do {
			++(hash_iter->current_index);
			current_slot += hash_iter->hash_slot_size;
		} while(empty_func(current_slot) && current_slot < last);
		hash_iter->current = (current_slot == last) ? NULL : next_func(current_slot, current_slot);
	}

	hash_iter->flags = HI_INVALID;
	
	return current;
}

static INLINE void* _hash_iter_move_prev(M_hash_iter* hash_iter, isemtpy_f_t empty_func, get_nextprev_f_t prev_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	
	void* current = hash_iter->current;
	M_sint8* current_slot = hash_slot_addr(hash, hash_iter->current_index);
	M_sint8 *head;

	move_prev_pos(hash_iter->current, current_slot, hash, hash_iter, head, empty_func, prev_func);

	hash_iter->flags = HI_INVALID;
	
	return current;
}

static INLINE void  _hash_iter_insert(M_hash_iter* hash_iter, void* stub, insert_f_t insert_func)
{	
	M_hash* hash = (M_hash*)hash_iter->container;
	M_size index = hash->hash_key(hash->get_key(stub)) % hash->ht_len;

	M_sint8 *current, *current_slot;

	if(index != hash_iter->current_index || !hash_iter->current)
	{
		hash_iter->current_index = index;
		current = hash_slot_addr(hash, index);
		current_slot = current;
	}
	else
	{
		current = hash_iter->current;
		current_slot = hash_slot_addr(hash, index);
	}

	insert_func(&current_slot, current, stub, hash);
	hash_iter->current = stub;
}


static INLINE void* _hash_iter_remove_self(M_hash_iter* hash_iter, remove_f_t remove_self_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	
	M_sint8* current = (M_sint8*)hash_iter->current;
	M_size index = hash_iter->current_index;
	M_sint8* current_slot = hash_slot_addr(hash, index);

	/*
		先移动current到下一个节点，然后取出当前节点，修改链表。
		当current不为空时取节点一定成功
		移除节点时一定要注意是否同行
	*/
	if(current)
	{
		if(hash_iter->flags != HI_LAST_CURSOR)
		{
			hash_iter->move_next(hash_iter);
			if(!hash_iter->current)
			{
				hash_iter->current = current;
				hash_iter->current_index = index;
				hash_iter->flags = HI_LAST_CURSOR;
			}
		}

		if(hash_iter->flags == HI_LAST_CURSOR)
			hash_iter->move_prev(hash_iter);


		remove_self_func(&current_slot, current, hash);
	}

	return current;
}

static INLINE void* _hash_iter_remove_next(M_hash_iter* hash_iter, remove_f_t remove_next_func, remove_f_t remove_self_func, is_headtail_f_t tail_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	
	M_sint8* current = (M_sint8*)hash_iter->current;
	M_size index = hash_iter->current_index;
	M_sint8* current_slot = hash_slot_addr(hash, index);
	M_sint8* ret = NULL;

	if(current)
	{
		if(tail_func(current_slot, current))	//current is tail of its list
		{
			hash_iter->move_next(hash_iter);
			if(hash_iter->current)
			{
				ret = hash_iter->current;
				current_slot = hash_slot_addr(hash, hash_iter->current_index);//(M_slist*)hash->ht + hash_iter->current_index;
				//ret = remove_next_func(&current_slot, current_slot, hash);
				remove_self_func(&current_slot, ret, hash);
				//current_slot->next = ret->next;
			}
			
			hash_iter->current = current;
			hash_iter->current_index = index;	
		}
		else
		{
			//ret = current->next;
			//current->next = ret->next;
			ret = remove_next_func(&current_slot, current, hash);
		}
	}
	return ret;
}


static INLINE void* _hash_iter_remove_prev(M_hash_iter* hash_iter, remove_f_t remove_prev_func, remove_f_t remove_self_func, is_headtail_f_t head_func)
{
	M_hash* hash = (M_hash*)hash_iter->container;
	
	M_sint8* current = (M_sint8*)hash_iter->current;
	M_size index = hash_iter->current_index;
	M_sint8* current_slot = hash_slot_addr(hash, index);
	M_sint8* ret = NULL;

	if(current)
	{
		if(head_func(current_slot, current))
		{
			hash_iter->move_prev(hash_iter);
			if(hash_iter->current)
			{
				ret = hash_iter->current;
				current_slot = hash_slot_addr(hash, hash_iter->current_index);
				remove_self_func(&current_slot, ret, hash);
			}
			
			hash_iter->current = current;
			hash_iter->current_index = index;	
		}
		else
		{
			ret = remove_prev_func(&current_slot, current, hash);
		}
	}
	return ret;
}
/*
	slist hash iterator implementation
*/
static INLINE void*	slist_hash_iter_begin(M_hash_iter* hash_iter)
{
	return _hash_iter_begin(hash_iter, slist_empty, slist_getnext);
}
static INLINE void* slist_hash_iter_end(M_hash_iter* hash_iter)
{
	return _hash_iter_end(hash_iter, slist_empty, slist_getprev);
}

static INLINE void* slist_hash_iter_move_next(M_hash_iter* hash_iter)
{
	return _hash_iter_move_next(hash_iter, slist_empty, slist_getnext);
}
static INLINE void* slist_hash_iter_move_prev(M_hash_iter* hash_iter)
{
	return _hash_iter_move_prev(hash_iter, slist_empty, slist_getprev);
}

static INLINE void  slist_hash_iter_insert_next(M_hash_iter* hash_iter, void* stub)
{	
	_hash_iter_insert(hash_iter, stub, (insert_f_t)slist_insert_next);
}

static INLINE void  slist_hash_iter_insert_prev(M_hash_iter* hash_iter, void* stub)
{
	_hash_iter_insert(hash_iter, stub, (insert_f_t)slist_insert_prev);
	//M_hash* hash = (M_hash*)hash_iter->container;
	//M_slist* current = (M_slist*)hash_iter->current;
	//M_slist* current_slot = hash_slot_addr(hash, hash_iter->current_index);
	//M_size index = hash->hash_key(hash->get_key(stub)) % hash->ht_len;
	//M_slist* prev;

	//if(index != hash_iter->current_index || !current)
	//{
	//	hash_iter->current_index = index;
	//	current_slot = hash_slot_addr(hash, index);
	//	current = current_slot;
	//}

	//prev = slist_getprev(current_slot, current);
	//prev->next = stub;
	//((M_slist*)stub)->next = current;
	//hash_iter->current = stub;
}

static INLINE void* slist_hash_iter_remove_self(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_self(hash_iter, (remove_f_t)slist_remove_self);
	//M_hash* hash = (M_hash*)hash_iter->container;
	//
	//M_slist* current = (M_slist*)hash_iter->current;
	//M_size index = hash_iter->current_index;
	//M_slist* current_slot = hash_slot_addr(hash, index);
	//M_slist *prev;

	///*
	//	先移动current到下一个节点，然后取出当前节点，修改链表。
	//	当current不为空时取节点一定成功
	//	移除节点时一定要注意是否同行
	//*/
	//if(current)
	//{
	//	if(hash_iter->flags != HI_LAST_CURSOR)
	//	{
	//		hash_iter->move_next(hash_iter);
	//		if(!hash_iter->current)
	//		{
	//			hash_iter->current = current;
	//			hash_iter->current_index = index;
	//			hash_iter->flags = HI_LAST_CURSOR;
	//		}
	//	}

	//	if(hash_iter->flags == HI_LAST_CURSOR)
	//		hash_iter->move_prev(hash_iter);

	//	prev = slist_getprev(current_slot, current);
	//	prev->next = current->next;
	//}

	//return current;
}

static INLINE void* slist_hash_iter_remove_next(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_next(hash_iter, (remove_f_t)slist_remove_next, (remove_f_t)slist_remove_self, slist_istail);
	//M_hash* hash = (M_hash*)hash_iter->container;
	//
	//M_slist* current = (M_slist*)hash_iter->current;
	//M_size index = hash_iter->current_index;
	//M_slist* current_slot = hash_slot_addr(hash, index);
	//M_slist* ret = NULL;

	//if(current)
	//{
	//	if( current->next == current_slot)	//current is tail of its list
	//	{
	//		hash_iter->move_next(hash_iter);
	//		if(hash_iter->current)
	//		{
	//			ret = hash_iter->current;
	//			current_slot = (M_slist*)hash->ht + hash_iter->current_index;
	//			current_slot->next = ret->next;
	//		}
	//		
	//		hash_iter->current = current;
	//		hash_iter->current_index = index;	
	//	}
	//	else
	//	{
	//		ret = current->next;
	//		current->next = ret->next;
	//	}
	//}
	//return ret;
}
static INLINE void* slist_hash_iter_remove_prev(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_prev(hash_iter, (remove_f_t)slist_remove_prev, (remove_f_t)slist_remove_self, slist_ishead);
	//M_hash* hash = (M_hash*)hash_iter->container;
	//
	//M_slist* current = (M_slist*)hash_iter->current;
	//M_size index = hash_iter->current_index;
	//M_slist* current_slot = hash_slot_addr(hash, index);
	//M_slist* ret = NULL;
	//M_slist* prev;

	//if(current)
	//{
	//	prev = slist_getprev(current_slot, current);
	//	if(prev == current_slot)	//current is head of its list
	//	{
	//		hash_iter->move_prev(hash_iter);
	//		if(hash_iter->current)
	//		{
	//			ret = hash_iter->current;
	//			current_slot = (M_slist*)hash->ht + hash_iter->current_index;
	//			prev = slist_getprev(current_slot, ret);
	//			prev->next = ret->next;
	//		}
	//		
	//		hash_iter->current = current;
	//		hash_iter->current_index = index;	
	//	}
	//	else
	//	{
	//		ret = prev;
	//		prev = slist_getprev(current_slot, ret);
	//		prev->next = ret->next;
	//	}
	//}
	//return ret;
}

/*
	dlist hash iterator implementation
*/
static INLINE void*	dlist_hash_iter_begin(M_hash_iter* hash_iter)
{
	return _hash_iter_begin(hash_iter, dlist_empty, dlist_getnext);
}
static INLINE void* dlist_hash_iter_end(M_hash_iter* hash_iter)
{
	return _hash_iter_end(hash_iter, dlist_empty, dlist_getprev);
}

static INLINE void* dlist_hash_iter_move_next(M_hash_iter* hash_iter)
{
	return _hash_iter_move_next(hash_iter, dlist_empty, dlist_getnext);
}
static INLINE void* dlist_hash_iter_move_prev(M_hash_iter* hash_iter)
{
	return _hash_iter_move_prev(hash_iter, dlist_empty, dlist_getprev);
}

static INLINE void  dlist_hash_iter_insert_next(M_hash_iter* hash_iter, void* stub)
{	
	_hash_iter_insert(hash_iter, stub, (insert_f_t)dlist_insert_next);
}

static INLINE void  dlist_hash_iter_insert_prev(M_hash_iter* hash_iter, void* stub)
{
	_hash_iter_insert(hash_iter, stub, (insert_f_t)dlist_insert_prev);
}

static INLINE void* dlist_hash_iter_remove_self(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_self(hash_iter, (remove_f_t)dlist_remove_self);
}

static INLINE void* dlist_hash_iter_remove_next(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_next(hash_iter, (remove_f_t)dlist_remove_next, (remove_f_t)dlist_remove_self, dlist_istail);
}
static INLINE void* dlist_hash_iter_remove_prev(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_prev(hash_iter, (remove_f_t)dlist_remove_prev, (remove_f_t)dlist_remove_self, dlist_ishead);
}

/*
	rbtree hash iterator implementation
*/
static INLINE void*	bst_hash_iter_begin(M_hash_iter* hash_iter)
{
	return _hash_iter_begin(hash_iter, bst_empty, bst_getnext);
}
static INLINE void* bst_hash_iter_end(M_hash_iter* hash_iter)
{
	return _hash_iter_end(hash_iter, bst_empty, bst_getprev);
}

static INLINE void* bst_hash_iter_move_next(M_hash_iter* hash_iter)
{
	return _hash_iter_move_next(hash_iter, bst_empty, bst_getnext);
}
static INLINE void* bst_hash_iter_move_prev(M_hash_iter* hash_iter)
{
	return _hash_iter_move_prev(hash_iter, bst_empty, bst_getprev);
}

static INLINE void  bst_hash_iter_insert_next(M_hash_iter* hash_iter, void* stub)
{	
	_hash_iter_insert(hash_iter, stub, (insert_f_t)_bst_insert);
}

static INLINE void  bst_hash_iter_insert_prev(M_hash_iter* hash_iter, void* stub)
{
	_hash_iter_insert(hash_iter, stub, (insert_f_t)_bst_insert);
}

static INLINE void* bst_hash_iter_remove_self(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_self(hash_iter, (remove_f_t)bst_remove_self);
}

static INLINE void* bst_hash_iter_remove_next(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_next(hash_iter, (remove_f_t)bst_remove_next, (remove_f_t)bst_remove_self, bst_istail);
}
static INLINE void* bst_hash_iter_remove_prev(M_hash_iter* hash_iter)
{
	return _hash_iter_remove_prev(hash_iter, (remove_f_t)bst_remove_prev, (remove_f_t)bst_remove_self, bst_ishead);
}

static INLINE void hash_iter_attach(M_hash_iter* hash_iter, M_hash* container)
{
	iter_attach((M_iter*)hash_iter, container);

	if(container->hash_type == HT_SLIST)
	{
		hash_iter->begin = slist_hash_iter_begin;
		hash_iter->end = slist_hash_iter_end;
		hash_iter->move_next = slist_hash_iter_move_next;
		hash_iter->move_prev = slist_hash_iter_move_prev;

		hash_iter->insert_next = slist_hash_iter_insert_next;
		hash_iter->insert_prev = slist_hash_iter_insert_prev;
		
		hash_iter->remove_self = slist_hash_iter_remove_self;
		hash_iter->remove_prev = slist_hash_iter_remove_prev;
		hash_iter->remove_next = slist_hash_iter_remove_next;

		hash_iter->hash_slot_size = sizeof(M_slist);
	}
	else if(container->hash_type == HT_DLIST)
	{
		hash_iter->begin = dlist_hash_iter_begin;
		hash_iter->end = dlist_hash_iter_end;
		hash_iter->move_next = dlist_hash_iter_move_next;
		hash_iter->move_prev = dlist_hash_iter_move_prev;

		hash_iter->insert_next = dlist_hash_iter_insert_next;
		hash_iter->insert_prev = dlist_hash_iter_insert_prev;
		
		hash_iter->remove_self = dlist_hash_iter_remove_self;
		hash_iter->remove_prev = dlist_hash_iter_remove_prev;
		hash_iter->remove_next = dlist_hash_iter_remove_next;

		hash_iter->hash_slot_size = sizeof(M_dlist);
	}
	else if(container->hash_type == HT_RBTREE)
	{
		hash_iter->begin = bst_hash_iter_begin;
		hash_iter->end = bst_hash_iter_end;
		hash_iter->move_next = bst_hash_iter_move_next;
		hash_iter->move_prev = bst_hash_iter_move_prev;

		hash_iter->insert_next = bst_hash_iter_insert_next;
		hash_iter->insert_prev = bst_hash_iter_insert_prev;
		
		hash_iter->remove_self = bst_hash_iter_remove_self;
		hash_iter->remove_prev = bst_hash_iter_remove_prev;
		hash_iter->remove_next = bst_hash_iter_remove_next;

		hash_iter->hash_slot_size = sizeof(M_bst_stub*);
	}

	hash_iter_set_bucket(hash_iter, container->ht_len, 0);

	return;
}
