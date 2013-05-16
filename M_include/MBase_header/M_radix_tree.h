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

/*
	longest/exact: returned node must be valid
	longest_ex: returned node maybe invalid
*/
#define RT_MODE_LONGEST		0
#define RT_MODE_EXACT		1
#define RT_MODE_LONGEST_EX	2

typedef struct st_rt_stub
{
	M_bst_stub			branch_stub;	//branch stub, parent use it
	M_sint8*			skey;			//string key, it points to (offset of) a string of its wrapper structure in valid node
										//trick: points to branches->skey - current_node->skey_len in invalid node
	struct st_rt_stub*	parent;
	M_bst_stub*			branches;		//children here

	//M_rt_stub*		child;	//trick: child pointer is no longer need, because this stub lays in child node, see get_rt_node macro
	//M_sint8			ckey;	//trick: ckey is just skey[0]
	M_sint8				color;
	M_bulletin8			flag;			//to identify the node is valid(user inserted node) or not(splitted node)
	M_sint16			skey_len;		//length of string from position of skey
	
} M_rt_stub;

typedef struct st_rt_arg
{
	M_rt_stub*	dummy_node;		//always not NULL before calling insert/remove/removenode
	M_rt_stub*	extra_node;		//always NULL before calling insert/remove/removenode
} M_rt_arg;

/*
	M_rt_init_node:
		init node before insert.
		key_len could be 0, means that key is a common C string ended by '\0'
		this function automatically set node valid.

	M_rt_search:
		key: to be searched string
		key_len: can be 0, means that key is a common C string ended by '\0'
		mode: longest match / exact match
		matched_len: [out], return how many chars that matched if search successfully

	M_rt_insert:
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

	   callback get_key is used to get key address of wrapper structure

   M_rt_remove:
		there are 5 cases when remove a node from radix tree:
		1. key is not found, return NULL
		   BTW: M_rt_remove returns removed node if remove successfully, returns NULL if key is not found
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

	M_rt_removenode: similar with M_rt_remove, but case 1 will never happen

	M_rt_freeall: free a radix tree. free_node is a callback written by user to free user node that wrappers M_rt_stub
				  pool points to a probable-used memory pool, which is needed by free_node
		
*/
MBASE_API	void		M_rt_init_node(M_rt_stub* node, M_sint8* key, M_sint32 key_len);
MBASE_API	M_rt_stub*	M_rt_search(M_rt_stub* root, M_sint8* key, M_sint32 key_len, M_sint32 mode, M_sint32* matched_len);
MBASE_API	M_rt_stub*	M_rt_insert(M_rt_stub** root, M_rt_stub* insert_node, M_rt_arg* arg/*, get_key_t get_key*/);
MBASE_API	M_rt_stub*	M_rt_remove(M_rt_stub** root, M_sint8* key, M_sint32 key_len, M_rt_arg* arg);
MBASE_API	M_rt_stub*	M_rt_removenode(M_rt_stub** root, M_rt_stub* remove_node, M_rt_arg* arg/*, get_key_t get_key*/);
MBASE_API	void		M_rt_freeall(M_rt_stub** root, M_free_t free_node, void* pool);

#ifdef __cplusplus
}
#endif

#endif //__M_RADIX_TREE_H__