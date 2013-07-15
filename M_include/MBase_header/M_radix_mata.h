/* 
	M_radix_mata.h : a radix-tree-like structure, support wildcard to mimic multi-regular expression matching..

	key difference to normal radix tree:

	1.	element in radix mata are not single char only, could be multi-chars
	2.	wildcard is supported， so there could be several matching results to one input
	3.	matching result contains not only which rules are matched, but also where there are matched
		that means matching result needs dynamic memory allocation. stack pool is applied in matching
		to simplify memory management. i.e. user should provide a memory chunk, organized as a stackpool
		when matching is applied.

	kevin.zheng@gmail.com
	2013/06/17
*/

#ifndef __M_RADIX_MATA_H__
#define __M_RADIX_MATA_H__


#include "M_types.h"
#include "M_rbtree.h"
#include "M_utility.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
	radix mata inherits all advantages of radix tree, including its data structures:
	M_rt_stub, M_rt_arg

	while some members now have different meaning:
	M_rt_stub->skey: still key "string", here "string" is a sequence of rmt_element, i.e. a multi-char sequence
	M_rt_stub->skey_len: length of key "string", by rmt_element. so actural length of skey is skey_len * ele_size

	APIs of radix mata are similar with radix tree's, but implementations are different,
	now callbacks of key "string" comparison are introduced to meet different requirements

	ele_size in M_rmt_root could only be 1, 2, 4, 8
	ele_pow is its corresponding exponent of 2, i.e. 0, 1, 2, 3
*/

#define RM_MAX_ELE_SIZE		sizeof(void*)
#define rmt_root_DECLARE\
	M_sint16	ele_pow;\
	M_sint16	ele_size
	
typedef struct st_rmt_root
{
	rmt_root_DECLARE;
	M_rt_stub*	root;
} M_rmt_root;

/*
	ele_size in parameter list is actual size of multi-char, i.e. 1, 2, 4, 8
	rmt_init_root will translate it to exponent of 2
*/
MBASE_API	void rmt_init_root(M_rmt_root* root, M_sint32 ele_size);

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

   rmt_remove:
		there are 5 cases when remove a node from radix tree:
		1. key is not found, return NULL
		   BTW: rmt_remove returns removed node if remove successfully, returns NULL if key is not found
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

	rt_remove_node: similar with rmt_remove, but case 1 will never happen

	rt_free_all: free a radix tree. free_node is a callback written by user to free user node that wrappers M_rt_stub
				  pool points to a probable-used memory pool, which is needed by free_node
		
*/
MBASE_API	void		rmt_init_node(M_rmt_root* root, M_rt_stub* node, M_sint8* key, M_sint32 key_len);
MBASE_API	M_rt_stub*	rmt_search(M_rmt_root* root, M_sint8* key, M_sint32 key_len, M_sint32 mode, M_sint32* matched_len);
MBASE_API	M_rt_stub*	rmt_insert_node(M_rmt_root* root, M_rt_stub* insert_node, M_rt_arg* arg);
MBASE_API	M_rt_stub*	rmt_remove(M_rmt_root* root, M_sint8* key, M_sint32 key_len, M_rt_arg* arg);
MBASE_API	M_rt_stub*	rmt_remove_node(M_rmt_root* root, M_rt_stub* remove_node, M_rt_arg* arg/*, get_key_t get_key*/);
MBASE_API	void		rmt_free_all(M_rmt_root* root, M_free_t free_node, void* pool);
MBASE_API	void		rmt_free_all_p(M_rmt_root* root, M_rt_pool* rp);

#define rmt_init_pool		rt_init_pool
#define rmt_pool_attach		rt_pool_attach
#define rmt_destroy_pool	rt_destroy_pool

#define rmt_process_arg		rt_process_arg
#define	rmt_free_arg		rt_free_arg
#define	rmt_alloc			rt_alloc
#define	rmt_free			rt_free


/*
	previous are just radix tree for multi-char, that "m" in "rmt" means
	now we begin with radix mata...

	key difference between radix multi-char tree and radix mata:
		wildcard in radix mata must be a single node, whatever it is valid or not
	we have rm_matafy to do this..
	rm_matafy must be called immediately after rmt is built.

	after matafy, remove_node is not supported any more
	if someone wants to change radix mata, only way is to rebuild it

	after radix mata is built, rm_matafy must be called after any insertion to radix mata..

	radix mata function: rmt_match
	constraint: all wide-char in radix mata must be distinct except wildcard if user would apply rmt_match
*/

typedef struct st_rm_stub
{
	rt_stub_DECLARE;
	struct st_rm_stub*	parent;
	struct st_rm_stub*	wc_node;	//points to kid wildcard
	void*				rule;
} M_rm_stub;

typedef struct st_rm_root
{
	rmt_root_DECLARE;
	M_rm_stub*	root;
	M_sint8		wildcard[RM_MAX_ELE_SIZE];
} M_rm_root;

MBASE_API	void		rm_init_root(M_rm_root* root, M_sint32 ele_size, M_sint8* wildcard);
MBASE_API	void		rm_init_node(M_rm_root* root, M_rm_stub* node, M_sint8* key, M_sint32 key_len);
MBASE_API	M_sint32	rm_is_wildcard(M_rm_root* root, M_rm_stub* stub);

MBASE_API	M_rm_stub*	rm_insert_node(M_rmt_root* root, M_rt_stub* insert_node, M_rt_arg* arg, void* rule);

/*
	2 pools are used here:
	pool: persistent pool, new node inserted to radix mata will be alloced here
	tmp_sp: temporary pool, queue for traverse radix tree..
*/
MBASE_API	M_sint32	rm_matafy(M_rm_root* root, M_malloc_t mem_alloc, void* pool, M_stackpool* tmp_sp);

#define rm_init_pool	rt_init_pool
#define rm_pool_attach	rt_pool_attach
#define rm_destroy_pool	rt_destroy_pool

#define	rm_free_arg		rt_free_arg
#define	rm_free			rt_free

MBASE_API	void		rm_process_arg(M_rt_pool* pool, M_rt_arg* extra_arg);
MBASE_API	void*		rm_alloc(M_sint32 size, M_rt_pool* rp);


/*
	following definitions are for single match process, so temporary memory chunk is used
*/

typedef	struct st_rm_input
{
	M_slist		list_stub;
	M_sint8		pat[RM_MAX_ELE_SIZE];
} M_rm_input;

typedef struct st_rm_state
{
	M_dlist		match_stub;
	M_slist		tmp_stub;					//new added, new removed node first linked here, then insert into match_list
	M_sint32	enter_pos;					//pos relative to input string, keep position of input string that enters the state
	M_sint32	match_len;					//record match length of normal pattern
	M_rm_stub*	rm_stub;					//points to wildcard node of radix mata, for tracing back
	M_sint32	pos;						//current matching position of rm_stub->skey
	struct st_rm_state*	parent;				//parent state, must be wildcard node
} M_rm_state;

// rm_branch_t returns 0 if judge success, otherwise judge fail
typedef M_sint32	(*rm_branch_t)(M_rm_stub* parent, M_rm_stub* child);

typedef struct st_rm_match_handle
{
	M_stackpool	spool;			//memory pool
	M_slist		input_head;		//M_rm_input->list_stub list here
	M_sint32	input_len;		//node counts of input list
	M_sint8*	input_array;	//array version of input_head list
	M_dlist		match_head;		//M_rm_state->match_stub list here
	rm_branch_t	judge;			//a callback to judge a branch should be selected or not
} M_rm_handle;

typedef struct st_rm_result_node
{
	M_slist		res_stub;
	M_sint32	enter_pos;
	M_sint32	leave_pos;
} M_rm_result_node;

typedef struct st_rm_result
{
	M_slist		match_stub;
	M_slist		res_head;
	void*		rule;
} M_rm_result;

//a callback might be added in future...
MBASE_API	void		rm_init_handle(M_rm_handle* handle, void* mem_chunk, M_sintptr mem_size, rm_branch_t rm_branch);

// pattern must be added in order
MBASE_API	M_sint32	rm_add_pattern(M_rm_handle* handle, M_rm_root* root, M_sint8* pat);

// rm_insert_rule: to simplify creating radix_mata...
// when user needs to create a radix mata, he could call rmt_init_handle and rmt_add_pattern first
// with a pre-alloced memory chunk(for temporary use)
// after handle is created over, then call rmt_insert_rule to build radix mata
// after rmt_insert_rule is called, temporary memory(mem_chunk of rmt_init_handle) could be released
// the reason why rm_stub is necessary as a parameter, is that user could have a bigger structure that contains rm_stub
// btw: this function is not tested...
MBASE_API	M_sint32	rm_insert_rule(M_rm_root* root, M_rm_handle* handle, M_rm_stub* rm_stub, void* rule,
	M_malloc_t mem_alloc, M_free_t mem_free, void* pool);

// rm_match: key API...
MBASE_API	M_sint32	rm_match(M_rm_root* root, M_rm_handle* handle);

// parse result from rm_handle. memory for result list is from handle->spool
// rm_parse_result only parses result for wildcard, 
// while rm_parse_total_result parese result for both wildcard and normal patterns
// NOTE: multi match_results may happen to same rule
MBASE_API	M_sint32	rm_parse_result(M_rm_root* root, M_rm_handle* handle, M_slist* match_res);
MBASE_API	M_sint32	rm_parse_total_result(M_rm_root* root, M_rm_handle* handle, M_slist* match_res);

// print result parsed by parse_match_result, for test only, only valid when rule is 0 terminated string
// print_rule: 0 -- not print rule
//			   1 -- print rule, only valid if rule is 0 terminated string
MBASE_API	void		rm_print_result(M_rm_root* root, M_rm_handle* handle, M_slist* match_res, M_sint32 print_rule, FILE* fp);

#ifdef __cplusplus
}
#endif

#endif //__M_RADIX_MATA_H__