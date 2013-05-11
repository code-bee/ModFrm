/* 
	M_rbtree.h : 定义红黑树结构

	kevin.zheng@gmail.com
	2012/09/02
*/

#ifndef __M_RBTREE_H__
#define __M_RBTREE_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_bst_stub
{
	struct st_bst_stub* parent;
	struct st_bst_stub* left;
	struct st_bst_stub* right;
} M_bst_stub;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// binary searching tree(left < node <= right)
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

MBASE_API M_bst_stub* bst_successor(M_bst_stub* x);
MBASE_API M_bst_stub* bst_predecessor(M_bst_stub* x);

MBASE_API M_bst_stub* bst_get_first(M_bst_stub* root);
MBASE_API M_bst_stub* bst_get_last(M_bst_stub* root);

/*
	bst_search;		find node exact same with input key
	bst_search_gt:	find node exact same or just greater than input key. 
					NULL if and only if key is greater than all other nodes in the tree
	bst_search_lt:	find node exact same or just less than input key. 
					NULL if and only key is less than all other nodes in the tree
*/
MBASE_API M_bst_stub* bst_search(M_bst_stub* root, void* key, cmp_key_t cmp_key, get_key_t get_key);
MBASE_API M_bst_stub* bst_search_gt(M_bst_stub* root, void* key, cmp_key_t cmp_key, get_key_t get_key);
MBASE_API M_bst_stub* bst_search_lt(M_bst_stub* root, void* key, cmp_key_t cmp_key, get_key_t get_key);

/*
	bst_insert: always success
	bst_insearch: success if same key is not in tree yet, return 1, otherwise return 0
*/
MBASE_API void bst_insert(M_bst_stub** root, M_bst_stub* x, cmp_key_t cmp_key, get_key_t get_key);
MBASE_API M_sint32 bst_insearch(M_bst_stub** root, M_bst_stub* x, cmp_key_t cmp_key, get_key_t get_key);

//return: the node removed from binary searching tree. NULL means remove fail
MBASE_API M_bst_stub* bst_remove(M_bst_stub** root, void* key, cmp_key_t cmp_key, get_key_t get_key);
MBASE_API void bst_remove_node(M_bst_stub** root, M_bst_stub* x);

MBASE_API void bst_free_all(M_bst_stub** root, M_free_t free_node, void* pool);


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// red black tree
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

MBASE_API void rbt_insert(M_bst_stub** root, M_bst_stub* x, cmp_key_t cmp_key, get_key_t get_key, 
						  get_rbcolor_t get_rbcolor, set_rbcolor_t set_rbcolor);

MBASE_API M_sint32 rbt_insearch(M_bst_stub** root, M_bst_stub* x, cmp_key_t cmp_key, get_key_t get_key,
								get_rbcolor_t get_rbcolor, set_rbcolor_t set_rbcolor);

MBASE_API M_bst_stub* rbt_remove(M_bst_stub** root, void* key, cmp_key_t cmp_key, get_key_t get_key, 
								 get_rbcolor_t get_rbcolor, set_rbcolor_t set_rbcolor);
MBASE_API void rbt_remove_node(M_bst_stub** root, M_bst_stub* x, get_rbcolor_t get_rbcolor, set_rbcolor_t set_rbcolor);


#ifdef __cplusplus
}
#endif

#endif // __M_RBTREE_H__