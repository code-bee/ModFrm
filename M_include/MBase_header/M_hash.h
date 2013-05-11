/* 
	M_hash.h : 定义hash结构

	we have 3 way to solve hash collision: single list, double list, red/black tree
	so there are 3 hash tables. generally speaking, single list and red/black tree are mostly used.

	强烈推荐用迭代器遍历、修改hash表
	We STRONGLY recommend to explore/modify hash table by iterators

	kevin.zheng@gmail.com
	2012/09/09
*/

#ifndef __M_HASH_H__
#define __M_HASH_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
	all M_self parameters used in interface actually points to original object
	i.e. M_self parameter in hash_interface->init points to M_hash object
*/

/*
	hash definitions
*/

typedef M_size (* hash_key_t)(void* key);

// hash types
#define	HT_SLIST	0
#define	HT_DLIST	1
#define	HT_RBTREE	2

/*
	init: init hash table, determine hash type: slist hash/dlist hash/rbtree hash
	destroy: destroy hash table, does not free hash nodes
	get_table_size: get size of hash table, should be called before init, then malloc memory for hash table, 
					then call init. get_table_size does no matter with hash object, which is only determined by
					hash type
	set_func: called before functional functions(insert/insearch/remove/remove_key), set callback functions
	hash_type: hash type, i.e. HT_XXX macros

	hash_key: compute hash via key
	cmp_key: compare keys
	get_key: get key via hash stub
	get_color/set_color: valid if hash_type is 
*/
#define hash_DECLARE\
	M_sint32	(* init)(M_self*, void* hash_table, M_size hash_table_len, M_sint32 hash_type);\
	M_sint32	(* destroy)(M_self*);\
	\
	M_size		(* get_table_size)(M_sint32 hash_type, M_size hash_table_len);\
	void		(* set_func)(M_self*, hash_key_t, cmp_key_t, get_key_t, get_rbcolor_t, set_rbcolor_t);\
	\
	void		(* insert)(M_self*, void* hash_stub);\
	M_sint32	(* insearch)(M_self*, void* hash_stub);\
	void*		(* search)(M_self*, void* key);\
	void*		(* remove)(M_self*, void* key);\
	void*		(* remove_node)(M_self*, void* hash_stub);\
	\
	void*		ht;\
	M_size		ht_len;\
	M_sint32	hash_type;\
	\
	hash_key_t	hash_key;\
	cmp_key_t	cmp_key;\
	get_key_t	get_key;\
	get_rbcolor_t	get_color;\
	set_rbcolor_t	set_color


typedef struct st_hash
{
	hash_DECLARE;
} M_hash;

MBASE_API INLINE void M_hash_construct(M_hash* obj);


/*
	common iterator definition

	attach: attach to a container
	detach: detach from a container
	begin:  move "current" to begin of attached container
	end:	move "current" to end of attached container
	
	search: most containers support search method, their iterators also need support it 

	get_current: get current node, does not move cursor

	move_next: move cursor to next node, return current node
	move_prev: move cursor to prev node, return current node

	insert_next: add a new node after current node, move cursor to new added node
	insert_prev: add a new node before current node, move cursor to new added node
	insert operation fails if cursor of iterator is NULL, returns -1
	
	usually to many ordered containers, insert_next/insert_prev are not well supported because of order.
	to these containers, insert_next/insert_prev must be used carefully, normally together with "search":
	after get a proper position by search, compare inserted node with searched, 
	then decide to use insert_prev or insert_next
	
	REMEMBER::: USE INSERT_NEXT/INSERT_PREV CAREFULLY!!!

	remove_self: remove current node, move cursor to next node if next node exists; 
				 otherwise move cursor to prev node till container is empty
	remove_prev: remove prev node, does not move cursor
	remove_next: remove next node, does not move cursor

	container: container that iterator points to
	current: node that iterator now points to
*/
#define iter_DECLARE\
	void  (* attach)(M_self*, void* container);\
	void  (* detach)(M_self*);\
	void* (* get_current)(M_self*);\
	void* (* begin)(M_self*);\
	void* (* end)(M_self*);\
	\
	void* (* search)(M_self*, void* key);\
	void* (* move_next)(M_self*);\
	void* (* move_prev)(M_self*);\
	\
	void  (* insert_next)(M_self*, void* stub);\
	void  (* insert_prev)(M_self*, void* stub);\
	\
	void* (* remove_self)(M_self*);\
	void* (* remove_prev)(M_self*);\
	void* (* remove_next)(M_self*);\
	\
	void* container;\
	void* current

typedef struct st_iter
{
	iter_DECLARE;
} M_iter;

MBASE_API INLINE void M_iter_construct(M_iter* obj);

/*
	hash iterator definition

	bucket: when iterator explores in hash table, bucket is used to make a boundary of iterator,
	it is useful in multithread environment: each thread manages a part(i.e. a bucket) of a hash table.

	set_bucket: set size of a bucket, in that way hash table is splitted into ht_len/bucket_size buckets
	current_index: array index of hash table that current node is in. return 0 if set bucket success, -1 otherwise.

	hash_slot_size: size of each hash slot
	flag: indicates cursor is last? or first? or middle?
*/
#define hash_iter_DECLARE\
	iter_DECLARE;\
	M_sint32 (* set_bucket)(M_self*, M_size bucket_size, M_size bucket_id);\
	M_size	current_index;\
	M_size	bucket_size;\
	M_size	bucket_id;\
	M_size	bucket_begin;\
	M_size	bucket_end;\
	M_sint16	hash_slot_size;\
	M_sint16	flags

typedef struct st_hash_iter
{
	hash_iter_DECLARE;
} M_hash_iter;

MBASE_API INLINE void M_hash_iter_construct(M_hash_iter* obj);

#ifdef __cplusplus
}
#endif

#endif	//__M_HASH_H__