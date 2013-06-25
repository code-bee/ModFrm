/* 
	M_radix_tree.h : radix tree. some tricks are used to avoid memory allocation in this algorithm

	kevin.zheng@gmail.com
	2013/05/11
*/

#ifndef __M_RADIX_TREE_H__
#define __M_RADIX_TREE_H__


#include "M_types.h"
#include "M_rbtree.h"
#include "M_utility.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
	used in M_rt_stub->flag, as bit indicator
*/
#define	RT_NODE_VALID	0x01
#define RT_NODE_INVALID	0x00

/*
	longest/exact: returned node must be valid
	longest_ex: returned node maybe invalid
*/
#define RT_MODE_LONGEST		0
#define RT_MODE_EXACT		1
#define RT_MODE_LONGEST_EX	2

#define rt_stub_DECLARE\
	/* branch stub, parent use it */\
	M_bst_stub			branch_stub;\
	/* string key, it points to (offset of) a string of its wrapper structure in valid node */\
	/* trick: points to branches->skey - current_node->skey_len in invalid node */\
	M_sint8*			skey;\
	\
	/* children here */\
	M_bst_stub*			branches;\
	\
	M_sint8				color;\
	/* to identify the node is valid(user inserted node) or not(splitted node) */\
	M_bulletin8			flag;\
	/* length of string from position of skey */\
	M_sint16			skey_len

typedef struct st_rt_stub
{
	rt_stub_DECLARE;
	struct st_rt_stub*	parent;
} M_rt_stub;

typedef struct st_rt_arg
{
	M_rt_stub*	dummy_node;		//always not NULL before calling insert/remove/removenode
	M_rt_stub*	extra_node;		//always NULL before calling insert/remove/removenode
} M_rt_arg;

/*
	rt_init_node:
		init node before insert.
		key_len could be 0, means that key is a common C string ended by '\0'
		this function automatically set node valid.

	rt_search:
		key: to be searched string
		key_len: can be 0, means that key is a common C string ended by '\0'
		mode: longest match / exact match
		matched_len: [out], return how many chars that matched if search successfully

	rt_insert_node:
		there are 5 cases when insert a node to radix tree:
		1. a node with exactly same key is already in radix tree
			1) the node is invalid, then insert_node replace it. the invalid node returns via arg->extra_node
			2) the node is valid, insert fail. the valid node returns via return value.
			   BTW: if insert successfully, the function returns NULL
	    2. P totally matches insert_node, but there are still chars in insert_node not matched
			1) insert_node is inserted as P's child
		3. insert_node totally matches P in radix tree, but there are still chars in P not matched
			1) insert_node will be parent of P
		4. both insert_node and P are partly matched
			1) arg->dummy_node will be inserted as P's parent, both insert_node and P become dummy_node's children
	   if dummy_node is used, NULL is set to inform user, user should manage all the cases to avoid memory leak

	   return NULL if insert success, otherwise conflict node in radix tree will be returned

   rt_remove:
		there are 5 cases when remove a node from radix tree:
		1. key is not found, return NULL
		   BTW: rt_remove returns removed node if remove successfully, returns NULL if key is not found
	    2. remove_node is leaf node
			a) its parent P is valid, remove remove_node is enough
			b) P is invalid. in this case, P must has more than 1 child.
			   if P has more than 2 children, remove remove_node is OK
			   if P has just 2 children, P will also be removed, another child of P becomes child of P's parent
			   because child number of P's parent does not change, analysis ends here.
		    so if remove_node is a leaf node, there are 2 cases:
			1) parent P is valid, or parent P is invalid, and has more than 2 children, remove remove_node is OK
			2) P is invalid, and has just 2 children, P will be returned via arg->extra_node,
			   and remove_node is also returned via return value
		3. remove_node has 1 child
			let the child be child of remove_node's parent, then remove remove_node
		4. remove node has more than 1 child
			replace remove_node with arg->dummy_node, then return remove_node

	rt_remove_node: similar with rt_remove, but case 1 will never happen

	rt_free_all: free a radix tree. free_node is a callback written by user to free user node that wrappers M_rt_stub
				  pool points to a probable-used memory pool, which is needed by free_node
		
*/
MBASE_API	void		rt_init_node(M_rt_stub* node, M_sint8* key, M_sint32 key_len);
MBASE_API	M_rt_stub*	rt_search(M_rt_stub* root, M_sint8* key, M_sint32 key_len, M_sint32 mode, M_sint32* matched_len);
MBASE_API	M_rt_stub*	rt_insert_node(M_rt_stub** root, M_rt_stub* insert_node, M_rt_arg* arg/*, get_key_t get_key*/);
MBASE_API	M_rt_stub*	rt_remove(M_rt_stub** root, M_sint8* key, M_sint32 key_len, M_rt_arg* arg);
MBASE_API	M_rt_stub*	rt_remove_node(M_rt_stub** root, M_rt_stub* remove_node, M_rt_arg* arg/*, get_key_t get_key*/);
MBASE_API	void		rt_free_all(M_rt_stub** root, M_free_t free_node, void* pool);

MBASE_API	INLINE	M_sint32	rt_valid(M_rt_stub* node);

/*
	following APIs and macros are for M_radix_mata only, please do NOT use them
*/
MBASE_API	INLINE void set_rt_valid(M_rt_stub* stub);
MBASE_API	INLINE void set_rt_invalid(M_rt_stub* stub);
MBASE_API	INLINE void replace_parent(M_bst_stub* bst_stub, void* parent);
MBASE_API	INLINE void update_rt_children(M_rt_stub* c);

#define get_rt_node(rbt_stub)	container_of(rbt_stub, M_rt_stub, branch_stub)


/*
	interface integrated with poolinf, so user may not care memory alloc/free with these APIs
*/
typedef struct st_rt_pool
{
	M_poolinf	valid_pool;
	M_poolinf	invalid_pool;
	M_sint32	stub_offset;
} M_rt_pool;

#define	rt_search_p	rt_search

MBASE_API	void		rt_init_pool(M_rt_pool* rp, M_sint32 stub_offset, M_sint32 max_nr_blocks);
MBASE_API	void		rt_pool_attach(M_rt_pool* rp, void* pool, M_malloc_t fp_alloc, M_free_t fp_free);
MBASE_API	void		rt_destroy_pool(M_rt_pool* rp);

MBASE_API	void		rt_process_arg(M_rt_pool* rp, M_rt_arg* extra_arg);
MBASE_API	void		rt_free_arg(M_rt_pool* rp, M_rt_arg* extra_arg);
/*
	only free nodes from radix tree, these "freed" nodes are kept in poolinf
*/
MBASE_API	void		rt_free_all_p(M_rt_stub** root, M_rt_pool* rp);

/*
	only 2 sizes are supported:
		size of invalid node: sizeof(M_rt_stub)
		size of valid node: sizeof(user_type_node), where stub_offset is offset of M_rt_stub in the structure

	a delimma is the what to return of rt_alloc, and how to set parameter mem of rt_free
	finally we choose return base address of memory allocated by rt_alloc, 
	that means there are two types of memory that return by rt_malloc: M_rt_stub, and user structure that contains M_rt_stub
	but addresses deliver to rt_free are all address of M_rt_stub, is inconsistent with return value of rt_alloc

	it's not a good idea to return only address of M_rt_stub in rt_alloc, 
	because user needs to shift the pointer to get base address of wrapper structure

	it's also not a good idea to deliver only base address to rt_free, 
	because system needs to add flag to distinguish what type of the memory is

	so that's why our last choice is not symmetric. it's also the intrinsic defect of poolinf: 
	can only support fixed size memory blocks, users need to deal many many things
*/
MBASE_API	void*		rt_alloc(M_sint32 size, M_rt_pool* rp);
MBASE_API	void		rt_free(M_rt_stub* mem, M_rt_pool* rp);

#ifdef __cplusplus
}
#endif

#endif //__M_RADIX_TREE_H__