/* 
	M_radix_mata.c : a radix-tree-like structure, support wildcard to mimic multi-regular expression matching..

	kevin.zheng@gmail.com
	2013/06/17
*/

#include "../MBase_priv.h"
#include "MBase.h"

void rmt_init_root(M_rmt_root* root, M_sint32 ele_size)
{
	root->ele_size = ele_size;
	root->root = NULL;
	switch(ele_size)
	{
	case 1:
		root->ele_pow = 0;
		break;
	case 2:
		root->ele_pow = 1;
		break;
	case 4:
		root->ele_pow = 2;
		break;
	case 8:
		root->ele_pow = 3;
		break;
	default:
		printf("Warning: error ele_size: %d\n", ele_size);
		root->ele_pow = 0;
		root->ele_size = 1;
		break;
	}
	
	//memcpy(root->wildcard, wildcard, ele_size);
}

static INLINE M_sintptr	cmp_key_rmt_1(void* key1, void* key2)
{
	return (M_sintptr)(*(M_sint8*)key1 - *(M_sint8*)key2);
}
static INLINE M_sintptr	cmp_key_rmt_2(void* key1, void* key2)
{
	return (M_sintptr)(*(M_sint16*)key1 - *(M_sint16*)key2);
}
static INLINE M_sintptr	cmp_key_rmt_4(void* key1, void* key2)
{
	return (M_sintptr)(*(M_sint32*)key1 - *(M_sint32*)key2);
}
static INLINE M_sintptr	cmp_key_rmt_8(void* key1, void* key2)
{
	return (M_sintptr)(*(M_sint64*)key1 - *(M_sint64*)key2);
}

static INLINE M_sintptr cmp_key_rmt(void* key1, void* key2, M_rmt_root* root)
{
	switch(root->ele_pow)
	{
	case 0:
		return cmp_key_rmt_1(key1, key2);
	case 1:
		return cmp_key_rmt_2(key1, key2);
	case 2:
		return cmp_key_rmt_4(key1, key2);
	case 3:
		return cmp_key_rmt_8(key1, key2);
	default:
		assert(0);
		return 0;
	}
}

static INLINE void*		get_key_rmt(void* stub)
{
	return get_rt_node(stub)->skey;
}
static INLINE M_sint32	get_rbcolor_rmt(void* stub)
{
	return (M_sint32)get_rt_node(stub)->color;
}
static INLINE void		set_rbcolor_rmt(void* stub, M_sint32 color)
{
	get_rt_node(stub)->color = (M_sint8)color;
}

/*
	for a specific string key, return how many leading chars that matches this key string in current radix tree node
*/
static INLINE M_sint32 search_node(M_rmt_root* root, M_rt_stub* node, M_sint8* key, M_sint32 key_len, M_sint32* key_pos)
{
	M_sint8* k1 = node->skey;
	M_sint32 i = 0;

	key += (*key_pos << root->ele_pow);

	while(i < node->skey_len && *key_pos < key_len && !cmp_key_rmt(k1, key, root))
	{
		++i;
		++(*key_pos);
		k1 += root->ele_size;
		key += root->ele_size;
	}
	return i;
}

/*
	for a specific char key, return a pointer to a radix tree node that starts with this key
*/
static INLINE M_rt_stub* search_branch(M_rmt_root* root, M_rt_stub* node, M_sint8* key)
{
	M_bst_stub* rbt_stub = NULL;
	
	switch(root->ele_pow)
	{
	case 0:
		rbt_stub = bst_search(node->branches, key, cmp_key_rmt_1, get_key_rmt);
		break;
	case 1:
		rbt_stub = bst_search(node->branches, key, cmp_key_rmt_2, get_key_rmt);
		break;
	case 2:
		rbt_stub = bst_search(node->branches, key, cmp_key_rmt_4, get_key_rmt);
		break;
	case 3:
		rbt_stub = bst_search(node->branches, key, cmp_key_rmt_8, get_key_rmt);
		break;
	}

	return rbt_stub ? get_rt_node(rbt_stub) : NULL;
}

static INLINE void update_rmt_parent(M_rt_stub* c, M_rmt_root* root)
{
	M_rt_stub* p = c->parent;
	if(p)
	{
		if(!rt_valid(p) && p->branches && p->skey)
			p->skey = get_rt_node(p->branches)->skey - (p->skey_len << root->ele_pow);
	}
	else
		root->root = c;
}

/*
	add node c to parent node p. 
	c must be out side of radix tree
	this function mainly process skey and skey_len of both p and cl
	
	it's user's responsibility to call update_rmt_parent and update_rmt_children

	following comments are obsoleted.
	//p and c meet following 2 conditions:
	//1. p->skey is a subset of c->skey, 
	//2. c->skey + pos matches p->skey + p->skey_len, 
	//   that means c->skey(pos - p->skey_len : pos) matches p->skey(0 : p->skey_len)
*/
static INLINE void add_rmt_node(M_rmt_root* root, M_rt_stub* p, M_rt_stub* c, M_sint8* key, M_sint16 key_len)
{
	c->parent = p;

	c->skey = key;
	c->skey_len = key_len;

	switch(root->ele_pow)
	{
	case 0:
		rbt_insert_node(&p->branches, &c->branch_stub, cmp_key_rmt_1, get_key_rmt, get_rbcolor_rmt, set_rbcolor_rmt);
		break;
	case 1:
		rbt_insert_node(&p->branches, &c->branch_stub, cmp_key_rmt_2, get_key_rmt, get_rbcolor_rmt, set_rbcolor_rmt);
		break;
	case 2:
		rbt_insert_node(&p->branches, &c->branch_stub, cmp_key_rmt_4, get_key_rmt, get_rbcolor_rmt, set_rbcolor_rmt);
		break;
	case 3:
		rbt_insert_node(&p->branches, &c->branch_stub, cmp_key_rmt_8, get_key_rmt, get_rbcolor_rmt, set_rbcolor_rmt);
		break;
	}
} 

/*
	insert_node + pos matches cut_node->parent + skey_len

	it has already updated parent, while update_children is not called yet
*/
static INLINE void cut_and_add_rmt_node(M_rt_stub* cut, M_rt_stub* insert, M_rmt_root* root, M_sint8* key, M_sint16 key_len)
{
	M_rt_stub* p = cut->parent;

	if(p && key_len)
	{
		rbt_remove_node(&(p->branches), &(cut->branch_stub), get_rbcolor_rmt, set_rbcolor_rmt);
		add_rmt_node(root, p, insert, key, key_len);
		update_rmt_parent(insert, root);
	}
	else
	{
		root->root = insert;
		insert->parent = NULL;
		
		insert->skey_len = key_len;
		if(key_len)
			insert->skey = key;
		else
			insert->skey = NULL;
	}
}

/*
	one of these 2 nodes must be valid, and the other must be invalid
	
	rmt_replace_node just changes the validation of rmt_node, only called when exact match happens

	update_parent and update_children are both not called yet
*/
static INLINE void rmt_replace_node(M_rmt_root* root, M_rt_stub* old_node, M_rt_stub* new_node)
{
	if(old_node->parent)
		rbt_replace_node(&old_node->parent->branches, &old_node->branch_stub, &new_node->branch_stub, get_rbcolor_rmt, set_rbcolor_rmt);
	else
		rbt_replace_node(NULL, &old_node->branch_stub, &new_node->branch_stub, get_rbcolor_rmt, set_rbcolor_rmt);
	new_node->parent = old_node->parent;
	new_node->branches = old_node->branches;

	if(rt_valid(new_node))
		new_node->skey += (new_node->skey_len - old_node->skey_len) << root->ele_pow;
	else
		new_node->skey = old_node->skey;

	new_node->skey_len = old_node->skey_len;
}

void		rmt_init_node(M_rmt_root* root, M_rt_stub* node, M_sint8* key, M_sint32 key_len)
{
	memset(node, 0, sizeof(M_rt_stub));
	node->skey = key;
	node->skey_len = (M_sint16)(key_len ? key_len : strlen(key)>>root->ele_pow);
	M_b8_init(&node->flag);
	M_b8_set(&node->flag, RT_NODE_VALID);
}

M_rt_stub*	rmt_search(M_rmt_root* root, M_sint8* key, M_sint32 key_len, M_sint32 mode, M_sint32* key_pos)
{
	M_rt_stub* t = root->root;
	M_rt_stub* p = NULL;			//t's parent, maybe invalid
	M_rt_stub* p_valid = NULL;		//t's ancester, valid node
	M_sint32 pos = 0;				//pos for node in radix tree
	*key_pos = 0;					//pos for key
	
	if(!t)
		return NULL;

	//it's dangerous in radix mata...
	if(!key_len)
		key_len = strlen(key) >> root->ele_pow;
	
	if(!t->skey)		// only root can has empty string key, it must be invaild in this case
		t = search_branch(root, t, &key[*key_pos << root->ele_pow]);

	while(t)
	{
		pos = search_node(root, t, key, key_len, key_pos);
		if(pos == t->skey_len)		//match all chars in node
		{
			if(*key_pos < key_len)		//but there are remain chars in the key
			{
				if(t->branches)		//there are still childrens, go on...
				{
					if(rt_valid(t))
						p_valid = t;
					p = t;
					t = search_branch(root, t, &key[*key_pos << root->ele_pow]);
				}
				else		//no childrens, match fail in exact match mode
				{
					t = NULL;
					break;
				}
			}
			else	//match successful
				break;
		}
		else	//only part of chars are successfully matched
		{
			t = NULL;
			break;
		}
	}

	switch(mode)
	{
	case RT_MODE_LONGEST:
		return t ? t : p_valid;
	case RT_MODE_EXACT:
		return t;
	case RT_MODE_LONGEST_EX:
		return t ? t : p;
	default:
		return NULL;
	}
}
M_rt_stub*	rmt_insert_node(M_rmt_root* root, M_rt_stub* insert_node, M_rt_arg* arg)
{
	M_rt_stub* t = root->root;
	M_rt_stub* p = NULL;		//parent of t
	
	M_sint32 key_pos = 0;		//pos for insert_node
	M_sint32 pos = 0;			//pos for node in radix tree
	M_sint32 skey_len = insert_node->skey_len;
	M_sint8* skey = insert_node->skey;

	if(!t)
	{		
		root->root = insert_node;
		return NULL;
	}

	if(!t->skey)		// only root can has empty string key, it must be invaild node in this case
	{
		p = t;
		if( !(t = search_branch(root, t, &skey[(key_pos << root->ele_pow)])) )	//case 0.0)
			t = p;
	}

	while(t)
	{
		pos = search_node(root, t, skey, skey_len, &key_pos);
		if(pos == t->skey_len)		//match all chars in node
		{
			if(key_pos < skey_len)		//but there are remain chars in the key
			{
				if(t->branches)		//there are still childrens, go on...
				{
					p = t;
					t = search_branch(root, t, &skey[key_pos << root->ele_pow]);	//if t is NULL, case 2.1)
				}
				else		//no childrens. insert node will be t's child, case 2.1)
					break;
			}
			else	//match successful
			{
				if(rt_valid(t))	//caes 1.2)
					return t;
				else				//case 1.1)
				{
					rmt_replace_node(root, t, insert_node);
					update_rmt_parent(insert_node, root);
					update_rt_children(insert_node);
					arg->extra_node = t;
					return NULL;
				}
			}
		}
		else	// node in radix tree is not totally matched, case 3 and 4
			break;
	}

	t = t ? t : p;

	//case 2.1), 3.1) and 4.1) are still left
	if(pos == t->skey_len)	// insert node, case 2.1) and 0.0)
	{
		add_rmt_node(root, t, insert_node, insert_node->skey + (key_pos << root->ele_pow), insert_node->skey_len - key_pos);
		update_rmt_parent(insert_node, root);
	}
	else
	{
		if(key_pos == skey_len)	// insert node totally matches, case 3.1)
		{
			cut_and_add_rmt_node(t, insert_node, root, insert_node->skey + ((key_pos - pos) << root->ele_pow), pos);
			add_rmt_node(root, insert_node, t, t->skey + (pos << root->ele_pow), t->skey_len - pos);
	}
		else	// both are not partly matches, case 4.1)
		{
			memset(arg->dummy_node, 0, sizeof(M_rt_stub));
			M_b8_init(&arg->dummy_node->flag);
			set_rt_invalid(arg->dummy_node);
			cut_and_add_rmt_node(t, arg->dummy_node, root, t->skey, pos);
			add_rmt_node(root, arg->dummy_node, t, t->skey + (pos << root->ele_pow), t->skey_len - pos);
			add_rmt_node(root, arg->dummy_node, insert_node, insert_node->skey + (key_pos << root->ele_pow), 
				insert_node->skey_len - key_pos);
			//update_rmt_children(arg->dummy_node);	//is not necessary here
			update_rmt_parent(t, root);

			arg->dummy_node = NULL;
		}
	}

	return NULL;
}

M_rt_stub*	rmt_remove(M_rmt_root* root, M_sint8* key, M_sint32 key_len, M_rt_arg* arg)
{
	M_sint32 key_pos;
	M_rt_stub* remove_node = rmt_search(root, key, key_len, RT_MODE_EXACT, &key_pos);

	return remove_node ? rmt_remove_node(root, remove_node, arg) : NULL;
}

M_rt_stub*	rmt_remove_node(M_rmt_root* root, M_rt_stub* remove_node, M_rt_arg* arg)
{
	M_rt_stub* tmp = NULL;
	M_rt_stub* p = remove_node->parent;

	if(!remove_node->branches)//rm is leaf
	{
		if(p)//if it has parent
		{
			rbt_remove_node(&p->branches, &remove_node->branch_stub, get_rbcolor_rmt, set_rbcolor_rmt);
			if(rt_valid(p) || bst_get_node_count_for_rt_tree(p->branches) > 1)	//case 2.1), invalid node must has more than 1 child
			{
				update_rmt_parent(remove_node, root);
			}
			else	//case 2.2). node must be invalid, and has just 2 children
			{
				tmp = get_rt_node(p->branches);
				cut_and_add_rmt_node(p, tmp, root, tmp->skey - (p->skey_len << root->ele_pow), tmp->skey_len + p->skey_len);
				//update_rmt_children(p->branches);	//is not necessary here

				arg->extra_node = p;
			}
		}
		else
			root->root = NULL;
	}
	else
	{
		if(bst_get_node_count_for_rt_tree(remove_node->branches) == 1)	// case 3.
		{
			tmp = get_rt_node(remove_node->branches);
			cut_and_add_rmt_node(remove_node, tmp, root, tmp->skey - (remove_node->skey_len << root->ele_pow), 
				tmp->skey_len + remove_node->skey_len);
			update_rt_children(remove_node->parent); 
		}
		else	// case 4.
		{
			M_b8_init(&arg->dummy_node->flag);
			set_rt_invalid(arg->dummy_node);
			rmt_replace_node(root, remove_node, arg->dummy_node);
			//following sequence is essential
			update_rt_children(arg->dummy_node);
			update_rmt_parent(get_rt_node(arg->dummy_node->branches), root);
			update_rmt_parent(arg->dummy_node, root);

			arg->dummy_node = NULL;
		}
	}

	return remove_node;

}

static INLINE void	add_to_free_node_list(M_bst_stub* bst_stub, M_rt_stub** rm_list_last, M_bst_stub** bst_list_last)
{
	(*rm_list_last)->parent = get_rt_node(bst_stub);
	*rm_list_last = (*rm_list_last)->parent;

	//printf("%s(%d) insert into rmt_node list\n", (*rm_list_last)->skey, (*rm_list_last)->skey_len);

	(*bst_list_last)->parent = bst_stub;
	(*bst_list_last) = (*bst_list_last)->parent;
}

void		rmt_free_all(M_rmt_root* root, M_free_t free_node, void* pool)
{
	rt_free_all(&root->root, free_node, pool);
}

void		rmt_free_all_p(M_rmt_root* root, M_rt_pool* rp)
{
	rmt_free_all(root, rt_free, rp);
}

void	rm_init_root(M_rm_root* root, M_sint32 ele_size, M_sint8* wildcard)
{
	rmt_init_root(root, ele_size);
	memcpy(root->wildcard, wildcard, root->ele_size);
}

void	rm_init_node(M_rm_root* root, M_rm_stub* node, M_sint8* key, M_sint32 key_len)
{
	rmt_init_node(root, node, key, key_len);
	node->wc_node = NULL;
	node->rule = NULL;
}

M_sint32	rm_is_wildcard(M_rm_root* root, M_rm_stub* stub)
{
#ifdef _DEBUG
	if(!cmp_key_rmt(root->wildcard, stub->skey, (M_rmt_root*)root))
		assert(stub->skey_len == 1);
#endif
	return (!cmp_key_rmt(root->wildcard, stub->skey, (M_rmt_root*)root));
}

void	rm_process_arg(M_rt_pool* pool, M_rt_arg* extra_arg)
{
	if(!extra_arg->dummy_node)
	{
		extra_arg->dummy_node = rt_alloc(sizeof(M_rm_stub), pool);
		((M_rm_stub*)extra_arg->dummy_node)->wc_node = NULL;
		M_b8_init(&extra_arg->dummy_node->flag);
		set_rt_invalid(extra_arg->dummy_node);
	}
	if(extra_arg->extra_node)
	{
		rt_free(extra_arg->extra_node, pool);
		extra_arg->extra_node = NULL;
	}
}

/*
	split stub at position i

	for example:

	string: a b c
	pos:    0 1 2

	when i == 0, split result: a / b c
	when i == 1, split result: a b / c	i.e. 1 = 3 - 2 = strlen-2
	when i == 2, nothing to split
*/
static INLINE M_rm_stub* rm_splitnode(M_rm_root* root, M_rm_stub* stub, M_sint32 i, M_malloc_t mem_alloc, void* pool)
{
	M_rm_stub*	add_node;
	M_sint32	skey_len = stub->skey_len;

	assert(i < skey_len - 1);
	++i;

	if( !(add_node = (M_rm_stub*)mem_alloc(sizeof(M_rm_stub), pool)))
		return NULL;

	rm_init_node(root, add_node, stub->skey, i);
	set_rt_invalid((M_rt_stub*)add_node);

	cut_and_add_rmt_node((M_rt_stub*)stub, (M_rt_stub*)add_node, (M_rmt_root*)root, stub->skey, i);
	add_rmt_node((M_rmt_root*)root, (M_rt_stub*)add_node, (M_rt_stub*)stub, 
		add_node->skey + (i << root->ele_pow), skey_len - i);

	return add_node;
}

/*
	好在进行matafy切割时，每个新增节点都只有一个父节点，一个子节点，所以简化了不少操作
	不过也正是因为如此，破坏了radix tree的结构，导致matafy之后remove和insert操作都不能做了，只能search
	可以在M_rm_root中加入标志，保证matafy了之后拒绝执行insert和remove
*/
static INLINE M_sint32 rm_matafy_node(M_rm_root* root, M_rm_stub* stub, M_malloc_t mem_alloc, void* pool)
{
	M_sint32	i = 1;	//i=0也可以，不过0是多余的，因为不会出现连续的通配
	M_rm_stub*	wc_node;


	if(stub->skey_len <= 1 || !stub->skey)
	{
		if(stub->skey_len == 1 && rm_is_wildcard(root, stub) && stub->parent)
			stub->parent->wc_node = stub;
		return 0;
	}

	//通常情况下面的逻辑有问题，不过在radix mata中由于不会出现连续的通配，所以这样是OK的
	
	if(!cmp_key_rmt(root->wildcard, stub->skey, (M_rmt_root*)root))
	{
		//wildcard seg: add_node1
		//left seg: stub
		if( !(wc_node = rm_splitnode(root, stub, 0, mem_alloc, pool)) )
			return -1;
		if(wc_node->parent)
			wc_node->parent->wc_node = wc_node;
		if(stub->skey_len <= 1)
			return 0;
	}
	
	if(!cmp_key_rmt(root->wildcard, &stub->skey[(stub->skey_len - 1)<<root->ele_pow], (M_rmt_root*)root))
	{
		//leading seg: add_node1
		//wildcard seg: stub
		wc_node = stub;
		if( !(stub = rm_splitnode(root, stub, stub->skey_len - 2, mem_alloc, pool)) )
			return -1;
		if(wc_node->parent)
			wc_node->parent->wc_node = wc_node;
		if(stub->skey_len <= 1)
			return 0;
	}

	//both i and stub->skey_len are changing during loop
	while(i < stub->skey_len)
	{
		if(!cmp_key_rmt(root->wildcard, &stub->skey[i<<root->ele_pow], (M_rmt_root*)root))
		{
			if( !rm_splitnode(root, stub, i-1, mem_alloc, pool) )
				return -1;
			if(stub->skey_len <= 1)
				return 0;

			if( !(wc_node = rm_splitnode(root, stub, 0, mem_alloc, pool)) )
				return -1;
			if(wc_node->parent)
				wc_node->parent->wc_node = wc_node;
			if(stub->skey_len <= 1)
				return 0;

			i = 1;
		}
		else
			++i;
	}
	return 0;
}

/*
	BFS...
*/
typedef struct st_rm_queue_node
{
	M_slist		q_stub;
	M_rm_stub*	rm_stub;
} rm_queue_node_t;
M_sint32	rm_matafy(M_rm_root* root, M_malloc_t mem_alloc, void* pool, M_stackpool* tmp_sp)
{
	//queue: NULL terminated single list
	M_slist*	queue_head = NULL;
	M_slist*	queue_tail = NULL;
	M_bst_stub*	bst_stub;
	rm_queue_node_t	*queue_node, *tmp_queue_node;

	if(!(queue_node = (rm_queue_node_t*)sp_alloc(sizeof(rm_queue_node_t), tmp_sp)))
		return -1;
	//add root to queue
	queue_node->rm_stub = root->root;
	queue_head = &queue_node->q_stub;
	queue_tail = queue_head;
	queue_tail->next = NULL;
	
	//BFS process queue
	while(queue_head)
	{
		queue_node = container_of(queue_head, rm_queue_node_t, q_stub);
		bst_stub = bst_get_first(queue_node->rm_stub->branches);
		while(bst_stub)
		{
			if(!(tmp_queue_node = (rm_queue_node_t*)sp_alloc(sizeof(rm_queue_node_t), tmp_sp)))
				return -1;
			tmp_queue_node->rm_stub = (M_rm_stub*)get_rt_node(bst_stub);
			queue_tail->next = &tmp_queue_node->q_stub;
			queue_tail = queue_tail->next;

			bst_stub = bst_successor(bst_stub);
		}
		queue_tail->next = NULL;

		if( rm_matafy_node(root, queue_node->rm_stub, mem_alloc, pool) < 0 )
			return -1;

		queue_head = queue_head->next;
	}

	return 0;
}

void*		rm_alloc(M_sint32 size, M_rt_pool* pool)
{
	if(size == sizeof(M_rm_stub))
		return pi_alloc(size, &pool->invalid_pool);
	else
		return pi_alloc(size, &pool->valid_pool);
}

void		rm_init_handle(M_rm_handle* handle, void* mem_chunk, M_sintptr mem_size, rm_branch_t rm_branch)
{
	handle->input_len = 0;
	handle->input_array = NULL;
	sp_init(mem_chunk, mem_size, &handle->spool);
	slist_init(&handle->input_head);
	dlist_init(&handle->match_head);
	handle->judge = rm_branch;
}

M_sint32	rm_add_pattern(M_rm_handle* handle, M_rm_root* root, M_sint8* pat)
{
	M_rm_input* input_node = (M_rm_input*)sp_alloc(sizeof(M_rm_input), &handle->spool);
	if(!input_node)
		return -1;

	memcpy(input_node->pat, pat, root->ele_size);
	slist_insert(&handle->input_head, &input_node->list_stub);
	++handle->input_len;
	return 0;
}

M_rm_stub*	rm_insert_node(M_rmt_root* root, M_rm_stub* insert_node, M_rt_arg* arg, void* rule)
{
	insert_node->rule = rule;
	return (M_rm_stub*)rmt_insert_node(root, (M_rt_stub*)insert_node, arg);
}

static INLINE void	rm_convert_inputlist(M_rm_handle* handle, M_rm_root* root, M_sint8* output_array)
{
	M_slist*	input_stub;
	M_rm_input* input_node;
	M_sint32	i = 0;

	//先逆序
	slist_reverse(&handle->input_head);
	input_stub = handle->input_head.next;
	while(input_stub != &handle->input_head)
	{
		input_node = container_of(input_stub, M_rm_input, list_stub);
		memcpy(output_array + (i<<root->ele_pow), input_node->pat, root->ele_size);
		input_stub = input_stub->next;
		++i;
	}
}

M_sint32	rm_insert_rule(M_rm_root* root, M_rm_handle* handle, M_rm_stub* rm_stub, void* rule,
	M_malloc_t mem_alloc, M_free_t mem_free, void* pool)
{
	M_sint8*	w_str = (M_sint8*)mem_alloc(handle->input_len << root->ele_pow, pool);
	M_rt_arg	arg;

	if(!w_str)
		return -1;

	arg.dummy_node = (M_rt_stub*)mem_alloc(sizeof(M_rm_stub), pool);
	arg.extra_node = NULL;
	
	rm_convert_inputlist(handle, root, w_str);

	rm_init_node(root, rm_stub, w_str, handle->input_len);
	rm_stub = (M_rm_stub*)rm_insert_node((M_rmt_root*)root, (M_rt_stub*)rm_stub, &arg, rule);

	if(arg.dummy_node)
		mem_free(arg.dummy_node, pool);
	if(arg.extra_node)
		mem_free(arg.extra_node, pool);

	if(rm_stub)
		return -1;
	else
		return 0;
}

static INLINE void	rm_state_init(M_rm_state* state, M_rm_state* parent, M_rm_stub* rm_stub, M_sint32 pos, M_sint32 key_pos)
{
	state->enter_pos = key_pos;
	state->match_len = pos;
	state->rm_stub = rm_stub;
	state->parent = parent;
	state->pos = pos;
}

/*
	查找通配节点的子节点中是否有和输入字符匹配的
*/
static INLINE M_sint32	rm_process_wc_child(M_rm_root* root, M_rm_handle* handle, M_rm_state* state, M_sint32 key_pos, M_slist* add_list)
{
	M_rm_stub*	t = NULL;
	M_sint8*	key = handle->input_array;
	M_rm_state* new_state;

	//通配节点不可能再有通配子节点
	assert(!state->rm_stub->wc_node);

	if( (t = (M_rm_stub*)search_branch((M_rmt_root*)root, (M_rt_stub*)state->rm_stub, &key[key_pos << root->ele_pow])) )
	{
		if(!handle->judge || !handle->judge(state->rm_stub, t))
		{
			if( !(new_state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
				return -1;
			rm_state_init(new_state, state, t, 1, key_pos);

			slist_insert(add_list, &new_state->tmp_stub);
			++state->match_len;
			//printf("add state at keypos %d, %s(%d, 0x%p) for childmatch of wildcard\n", key_pos, t->skey, t->skey_len, t);
		}
	}

	return 0;
}

static INLINE	void	rm_remove_state(M_rm_root* root, M_rm_state* state, M_slist* remove_list)
{
	M_rm_state* p = state->parent;
	while(p && !rm_is_wildcard(root, p->rm_stub))
		p = p->parent;

	if(p)
		--p->match_len;

	slist_insert(remove_list, &state->tmp_stub);
}

/*
	处理当前state，key_pos代表input_array的当前位置。
	实际上key_pos比state的位置要大一

	当前state有两种情况：通配/非通配
	当前为通配时可能要新增状态
	当前为非通配时检查其子节点，其子节点可能有通配，
	对子通配节点还需要检查其子节点中有无和当前输入字符匹配的，如果有，还需要添加状态
	
	对每个状态，enter_pos记录了进入该状态的输入字符位置
*/
static INLINE M_sint32	rm_match_state(M_rm_root* root, M_rm_handle* handle, M_rm_state* state, M_sint32 key_pos, M_slist* add_list, M_slist* remove_list)
{
	M_rm_stub*	t = NULL;
	M_sint8*	key = handle->input_array;
	M_rm_state	*new_state, *p;

	if(rm_is_wildcard(root, state->rm_stub))
	{
		if( rm_process_wc_child(root, handle, state, key_pos, add_list) < 0 )
			return -1;

		//通配节点pos成员无意义，始终是0
		//leave_pos始终比key_pos大1。自增leave_pos，表示通配节点匹配当前输入字符成功
		//++state->leave_pos;
	}
	else
	{
		if(state->pos == state->rm_stub->skey_len)
		{
			//如果有通配子节点，直接创建新状态加入。新加入状态不匹配任何字符(enter_pos = leave_pos)
			if(state->rm_stub->wc_node)
			{
				if( !(new_state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
					return -1;
				rm_state_init(new_state, state, state->rm_stub->wc_node, 0, key_pos);
				slist_insert(add_list, &new_state->tmp_stub);
				//printf("add state at keypos %d, %s(%d, 0x%p) for wildcard\n", key_pos, state->rm_stub->wc_node->skey, state->rm_stub->wc_node->skey_len, state->rm_stub->wc_node);

				//如果其父节点存在通配，将其从状态链表中摘去
				p = new_state->parent;
				while(p && !rm_is_wildcard(root, p->rm_stub))
					p = p->parent;
				if(p && !--p->match_len && !rt_valid((M_rt_stub*)p->rm_stub))
				{
					slist_insert(remove_list, &p->tmp_stub);
					//printf("remove state at keypos %d, %s(%d, 0x%p) for dup wildcard\n", key_pos, p->rm_stub->skey, p->rm_stub->skey_len, p->rm_stub);
				}

				//用当前字符匹配新通配的子节点，看有无匹配
				if( rm_process_wc_child(root, handle, new_state, key_pos, add_list) < 0 )
					return -1;
			}

			if( (t = (M_rm_stub*)search_branch((M_rmt_root*)root, (M_rt_stub*)state->rm_stub, &key[key_pos << root->ele_pow])) )
			{
				if(!handle->judge || !handle->judge(state->rm_stub, t))
				{
					++state->match_len;
					state->rm_stub = t;
					state->pos = 1;
					//printf("match successfully at keypos %d, %s(%d, 0x%p) for branch match\n", key_pos, state->rm_stub->skey, state->rm_stub->skey_len, state->rm_stub);
				}
				else
				{
					rm_remove_state(root, state, remove_list);
					//printf("remove state at keypos %d, %s(%d, 0x%p) for judge fail\n", key_pos, state->rm_stub->skey, state->rm_stub->skey_len, state->rm_stub);
				}
			}
			else
			{
				rm_remove_state(root, state, remove_list);
				//printf("remove state at keypos %d, %s(%d, 0x%p) for branch mismatch\n", key_pos, state->rm_stub->skey, state->rm_stub->skey_len, state->rm_stub);
			}
		}
		else
		{
			//如果下一个字符匹配成功，pos++，否则移除当前状态
			if(!cmp_key_rmt(&state->rm_stub->skey[state->pos << root->ele_pow], 
				&key[key_pos << root->ele_pow], (M_rmt_root*)root))
			{
				++state->pos;
				++state->match_len;
				//printf("match succesfully at key_pos %d, %s(%d, 0x%p) for node match\n", key_pos, state->rm_stub->skey, state->rm_stub->skey_len, state->rm_stub);
			}
			else
			{
				rm_remove_state(root, state, remove_list);
				//printf("remove state at keypos %d, %s(%d, 0x%p) for mismatch\n", key_pos, state->rm_stub->skey, state->rm_stub->skey_len, state->rm_stub);
			}
		}
	}

	//++(*key_pos);
	return 0;
}

/*
	匹配下一个字符
*/
static INLINE M_sint32	rm_match_char(M_rm_root* root, M_rm_handle* handle, M_sint32 key_pos)
{
	M_dlist*	list_stub = handle->match_head.next;
	M_slist		add_list, remove_list;
	M_slist*	tmp_stub;
	M_rm_state*	state;

	slist_init(&add_list);
	slist_init(&remove_list);

	while(list_stub != &handle->match_head)
	{
		if( rm_match_state(root, handle, container_of(list_stub, M_rm_state, match_stub), key_pos, &add_list, &remove_list) < 0 )
			return -1;
		list_stub = list_stub->next;
	}

	tmp_stub = add_list.next;
	while(tmp_stub != &add_list)
	{
		state = container_of(tmp_stub, M_rm_state, tmp_stub);
		dlist_insert(&handle->match_head, &state->match_stub);
		tmp_stub = tmp_stub->next;
	}

	tmp_stub = remove_list.next;
	while(tmp_stub != &remove_list)
	{
		state = container_of(tmp_stub, M_rm_state, tmp_stub);
		dlist_remove(&handle->match_head, &state->match_stub);
		tmp_stub = tmp_stub->next;
	}


	return 0;
}

/*
	事实上，匹配轨迹中只需要记录通配符对应的段即可
	一次匹配按以下逻辑工作：（仅供参考，最终实现和这里描述的不完全一样）
	1. 如果radix mata中的下一个节点不是通配
		a. 如果该节点和输入串中下一个字符相同，继续往下
		b. 如果该节点和输入串中下一个字符不同，回溯到最近的一个通配节点，取消其出口字符；
		   如果无最近通配节点，匹配失败退出
	2. 如果radix mata中的下一个节点是通配，记录下进入这个通配节点的输入串字符。
		a. 检查是通配节点否valid节点。如果valid，匹配结束
		b. 如果通配节点不是valid的，对输入串后续字符检索是否有出口
			1. 如果有出口，出去，记录下输入串在该通配节点的出口字符
			2. 如果没有出口，继续停留在通配节点处
	3. 如果输入串输入完毕，检查当前停留节点是否valid，如果valid，匹配成功，否则匹配失败

	最终输出结果中，记录所有匹配成功的规则的叶子节点，及路径上各通配的入口、出口字符

	rm_match的匹配是逐字符进行的，而不是以节点为单位。目的是保证所有状态的进度一致
*/

M_sint32	rm_match(M_rm_root* root, M_rm_handle* handle)
{
	M_rm_state*	state, *new_state;

	M_rm_stub*	t = root->root;
	M_sint32	key_pos = 0;
	M_sint8*	key;
	M_slist		add_list;
	M_slist*	add_stub;
	M_dlist*	match_stub;

	if(!t)
		return 0;

	if( !(handle->input_array = (M_sint8*)sp_alloc(handle->input_len << root->ele_pow, &handle->spool)) )
		return -1;
	rm_convert_inputlist(handle, root, handle->input_array);
	key = handle->input_array;

	slist_init(&add_list);

	if(!t->skey)		// only root can has empty string key, it must be invaild in this case
	{
		if(t->wc_node)
		{
			//如果有通配子节点，直接创建新状态加入。新加入状态不匹配任何字符(enter_pos = leave_pos)
			if( !(state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
				return -1;
			rm_state_init(state, NULL, t->wc_node, 0, key_pos);
			dlist_append(&handle->match_head, &state->match_stub);
			//printf("add state at keypos %d, %s(%d, 0x%p) for root wildcard\n", key_pos, t->wc_node->skey, t->wc_node->skey_len, t);

			//用当前字符匹配新通配的子节点，看有无匹配
			if( rm_process_wc_child(root, handle, state, key_pos, &add_list) < 0 )
				return -1;

			if(add_list.next != &add_list)
				dlist_append(&handle->match_head, &(container_of(add_list.next, M_rm_state, tmp_stub)->match_stub));
		}
		if( (t = (M_rm_stub*)search_branch((M_rmt_root*)root, (M_rt_stub*)t, &key[key_pos << root->ele_pow])) )
		{
			if(!handle->judge || !handle->judge(root->root, t))
			{
				if( !(state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
					return -1;
				rm_state_init(state, NULL, t, 1, key_pos);
				dlist_append(&handle->match_head, &state->match_stub);
				//printf("add state at keypos %d, %s(%d, 0x%p) for root branchmatch\n", key_pos, t->skey, t->skey_len, t);
			}
		}
	}
	else
	{
		if(rm_is_wildcard(root, t))
		{
			//如果有通配子节点，直接创建新状态加入。新加入状态不匹配任何字符(enter_pos = leave_pos)
			if( !(state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
				return -1;
			rm_state_init(state, NULL, t, 0, key_pos);
			dlist_append(&handle->match_head, &state->match_stub);
			//printf("add state at keypos %d, %s(%d, 0x%p) for root wildcard\n", key_pos, t->skey, t->skey_len, t);

			//用当前字符匹配新通配的子节点，看有无匹配
			if( rm_process_wc_child(root, handle, state, key_pos, &add_list) < 0 )
				return -1;

			if(add_list.next != &add_list)
				dlist_append(&handle->match_head, &(container_of(add_list.next, M_rm_state, tmp_stub)->match_stub));
		}
		else
		{
			//如果下一个字符匹配成功，pos++，否则移除当前状态
			if(!cmp_key_rmt(t->skey, key, (M_rmt_root*)root))
			{
				if( !(state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
					return -1;
				rm_state_init(state, NULL, t, 1, key_pos);
				dlist_append(&handle->match_head, &state->match_stub);
				//printf("add state at keypos %d, %s(%d, 0x%p) for root match\n", key_pos, t->skey, t->skey_len, t);
			}
		}
	}
	++key_pos;

	while(key_pos < handle->input_len)
	{
		rm_match_char(root, handle, key_pos);
		++key_pos;
	}

	slist_init(&add_list);
	//检查各停留状态是否合法，并为各通配节点计算匹配长度
	match_stub = handle->match_head.next;
	while(match_stub != &handle->match_head)
	{
		state = container_of(match_stub, M_rm_state, match_stub);
		match_stub = match_stub->next;

		if(rm_is_wildcard(root, state->rm_stub))	//通配节点没有匹配长度一说
		{
			if(!rt_valid((M_rt_stub*)state->rm_stub))
				dlist_remove(&handle->match_head, &state->match_stub);
		}
		else
		{
			if(state->pos != state->rm_stub->skey_len)
				dlist_remove(&handle->match_head, &state->match_stub);
			else
			{
				//如果其有个通配子节点，且该通配子节点valid，该规则匹配成功。需要在匹配结果中加上该通配子节点
				t = state->rm_stub->wc_node;
				if(t && rt_valid((M_rt_stub*)t))
				{
					if( !(new_state = (M_rm_state*)sp_alloc(sizeof(M_rm_state), &handle->spool)) )
						return -1;
					rm_state_init(new_state, state, t, 0, key_pos);
					slist_insert(&add_list, &new_state->tmp_stub);
					new_state->enter_pos = key_pos;
					//printf("add state at keypos %d, %s(%d, 0x%p) for tail wildcard\n", key_pos, t->skey, t->skey_len, t);

				}
				if(!rt_valid((M_rt_stub*)state->rm_stub))
					dlist_remove(&handle->match_head, &state->match_stub);
			}
		}
	}

	add_stub = add_list.next;
	while(add_stub != &add_list)
	{
		state = container_of(add_stub, M_rm_state, tmp_stub);
		dlist_append(&handle->match_head, &state->match_stub);
		add_stub = add_stub->next;
	}

	return 0;
}

M_sint32	rm_parse_result(M_rm_root* root, M_rm_handle* handle, M_slist* match_res)
{
	M_dlist*	list_stub = handle->match_head.next;
	M_rm_state*	state;
	M_rm_result_node*	result_node;
	M_rm_result*		match_result;
	M_sint32	enter_pos = 0;
	M_rm_stub*	rm_stub;

	slist_init(match_res);

	while(list_stub != &handle->match_head)
	{
		if( !(match_result = (M_rm_result*)sp_alloc(sizeof(M_rm_result), &handle->spool)) )
			return -1;
		slist_init(&match_result->res_head);
		slist_insert(match_res, &match_result->match_stub);

		state = container_of(list_stub, M_rm_state, match_stub);
		while(state)
		{
			if(rm_is_wildcard(root, state->rm_stub))
			{
				if( !(result_node = (M_rm_result_node*)sp_alloc(sizeof(M_rm_result_node), &handle->spool)) )
					return -1;
				result_node->enter_pos = state->enter_pos;
				result_node->leave_pos = enter_pos ? enter_pos : handle->input_len;
				slist_insert(&match_result->res_head, &result_node->res_stub);
			}
			else
				enter_pos = state->enter_pos;
			state = state->parent;
		}
		list_stub = list_stub->next;
	}
	return 0;
}

M_sint32	rm_parse_total_result(M_rm_root* root, M_rm_handle* handle, M_slist* match_res)
{
	M_dlist*	list_stub = handle->match_head.next;
	M_rm_state*	state;
	M_rm_result_node*	result_node;
	M_rm_result*		match_result;
	M_sint32	enter_pos = 0;
	M_sint32	first_flag = 1;
	M_rm_stub*	rm_stub;

	slist_init(match_res);

	while(list_stub != &handle->match_head)
	{
		if( !(match_result = (M_rm_result*)sp_alloc(sizeof(M_rm_result), &handle->spool)) )
			return -1;
		
		slist_init(&match_result->res_head);
		slist_insert(match_res, &match_result->match_stub);
		first_flag = 1;

		if( (state = container_of(list_stub, M_rm_state, match_stub)) )
			match_result->rule = state->rm_stub->rule;

		while(state)
		{
			if( !(result_node = (M_rm_result_node*)sp_alloc(sizeof(M_rm_result_node), &handle->spool)) )
				return -1;
			result_node->enter_pos = state->enter_pos;
			result_node->leave_pos = first_flag ? handle->input_len : enter_pos;
			slist_insert(&match_result->res_head, &result_node->res_stub);
			enter_pos = state->enter_pos;

			state = state->parent;
			first_flag = 0;
		}
		list_stub = list_stub->next;
	}
	return 0;
}

void	rm_print_result(M_rm_root* root, M_rm_handle* handle, M_slist* match_res, M_sint32 print_rule, FILE* fp)
{
	M_rm_result*		result;
	M_rm_result_node*	result_node;
	M_slist*			match_stub = match_res->next;
	M_slist*			result_stub;
	M_sint32			i,j;

	while(match_stub != match_res)
	{
		result = container_of(match_stub, M_rm_result, match_stub);
		result_stub = result->res_head.next;
		if(print_rule)
			fprintf(fp, "rule: %s, ", result->rule);
		while(result_stub != &result->res_head)
		{
			result_node = container_of(result_stub, M_rm_result_node, res_stub);
			fprintf(fp, "([%d]", result_node->enter_pos);
			for(i=result_node->enter_pos; i<result_node->leave_pos; i++)
			{
				for(j=0; j<root->ele_size; j++)
					fprintf(fp, "%c", handle->input_array[(i<<root->ele_pow) + j]);
			}
			fprintf(fp, "[%d]) ", result_node->leave_pos);
			result_stub = result_stub->next;
		}
		fprintf(fp, "\n");
		match_stub = match_stub->next;
	}
}