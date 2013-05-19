/* 
	M_radix_tree.c : radix tree. a trick is used to avoid memory allocation in this algorithm

	kevin.zheng@gmail.com
	2013/05/11
*/

#include "../MBase_priv.h"
#include "MBase.h"

#define get_rt_node(rbt_stub)	container_of(rbt_stub, M_rt_stub, branch_stub)

#define cmp_key_rt	cmp_key_M_sint8
static INLINE void*		get_key_rt(void* stub)
{
	//return &(get_rt_node(stub)->skey[0]);
	return get_rt_node(stub)->skey;
}
static INLINE M_sint32	get_rbcolor_rt(void* stub)
{
	return (M_sint32)get_rt_node(stub)->color;
}
static INLINE void		set_rbcolor_rt(void* stub, M_sint32 color)
{
	get_rt_node(stub)->color = (M_sint8)color;
}

static INLINE M_sint32	is_rt_valid(M_rt_stub* stub)
{
	return (M_b8_get(&stub->flag, RT_NODE_VALID) == RT_NODE_VALID);
}

static INLINE void set_rt_valid(M_rt_stub* stub)
{
	M_b8_set(&stub->flag, RT_NODE_VALID);
}

static INLINE void set_rt_invalid(M_rt_stub* stub)
{
	M_b8_clear(&stub->flag, RT_NODE_VALID);
}

/*
	for a specific string key, return how many leading chars that matches this key string in current radix tree node
*/
static INLINE M_sint32 search_node(M_rt_stub* node, M_sint8* key, M_sint32 key_len, M_sint32* key_pos)
{
	M_sint8* k1 = node->skey;
	M_sint32 i = 0;

	key += *key_pos;

	while(i < node->skey_len && *key_pos < key_len && *k1 == *key)
	{
		++i;
		++(*key_pos);
		++k1;
		++key;
	}
	return i;
}


/*
	for a specific char key, return a pointer to a radix tree node that starts with this key
*/
static INLINE M_rt_stub* search_branch(M_rt_stub* node, M_sint8 key)
{
	M_bst_stub* rbt_stub = bst_search(node->branches, &key, cmp_key_rt, get_key_rt);

	return rbt_stub ? get_rt_node(rbt_stub) : NULL;
}

static INLINE void replace_parent(M_bst_stub* bst_stub, void* parent)
{
	get_rt_node(bst_stub)->parent = parent;
}


static INLINE void update_rt_parent(M_rt_stub* c, M_rt_stub** root)
{
	M_rt_stub* p = c->parent;
	if(p)
	{
		if(!is_rt_valid(p) && p->branches && p->skey)
			p->skey = get_rt_node(p->branches)->skey - p->skey_len;
	}
	else
		*root = c;
}
static INLINE void update_rt_children(M_rt_stub* c)
{
	if(c)
		bst_travel(c->branches, replace_parent, c);
}

/*
	add node c to parent node p. 
	c must be out side of radix tree
	this function mainly process skey and skey_len of both p and cl
	
	p and c meet following 2 conditions:
	1. p->skey is a subset of c->skey, 
	2. c->skey + pos matches p->skey + p->skey_len, 
	   that means c->skey(pos - p->skey_len : pos) matches p->skey(0 : p->skey_len)

	it's user's responsibility to call update_parent and update_children
*/
static INLINE void add_rt_node(M_rt_stub* p, M_rt_stub* c, M_sint8* key, M_sint16 key_len)
{
	c->parent = p;

	c->skey = key;
	c->skey_len = key_len;

	rbt_insert(&p->branches, &c->branch_stub, cmp_key_rt, get_key_rt, get_rbcolor_rt, set_rbcolor_rt);
} 

/*
	insert_node + pos matches cut_node->parent + skey_len

	it has already updated parent, while update_children is not called yet
*/
static INLINE void cut_and_add_rt_node(M_rt_stub* cut, M_rt_stub* insert, M_rt_stub** root, M_sint8* key, M_sint16 key_len)
{
	M_rt_stub* p = cut->parent;

	if(p && key_len)
	{
		rbt_remove_node(&(p->branches), &(cut->branch_stub), get_rbcolor_rt, set_rbcolor_rt);
		add_rt_node(p, insert, key, key_len);
		update_rt_parent(insert, root);
	}
	else
	{
		*root = insert;
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
	
	replace_rt_node just changes the validation of rt_node, only called when exact match happens

	update_parent and update_children are both not called yet
*/
static INLINE void replace_rt_node(M_rt_stub* old_node, M_rt_stub* new_node)
{
	if(old_node->parent)
		replace_rbt_node(&old_node->parent->branches, &old_node->branch_stub, &new_node->branch_stub, get_rbcolor_rt, set_rbcolor_rt);
	else
		replace_rbt_node(NULL, &old_node->branch_stub, &new_node->branch_stub, get_rbcolor_rt, set_rbcolor_rt);
	new_node->parent = old_node->parent;
	new_node->branches = old_node->branches;

	if(is_rt_valid(new_node))
		new_node->skey += new_node->skey_len - old_node->skey_len;
	new_node->skey_len = old_node->skey_len;
}

void		M_rt_init_node(M_rt_stub* node, M_sint8* key, M_sint32 key_len)
{
	memset(node, 0, sizeof(M_rt_stub));
	node->skey = key;
	node->skey_len = (M_sint16)(key_len ? key_len : strlen(key));
	M_b8_init(&node->flag);
	M_b8_set(&node->flag, RT_NODE_VALID);
}

M_rt_stub*	M_rt_search(M_rt_stub* root, M_sint8* key, M_sint32 key_len, M_sint32 mode, M_sint32* key_pos)
{
	M_rt_stub* t = root;
	M_rt_stub* p = NULL;			//t's parent, maybe invalid
	M_rt_stub* p_valid = NULL;		//t's ancester, valid node
	M_sint32 pos = 0;				//pos for node in radix tree
	*key_pos = 0;					//pos for key
	
	if(!t)
		return NULL;

	if(!key_len)
		key_len = strlen(key);
	
	if(!t->skey)		// only root can has empty string key, it must be invaild in this case
		t = search_branch(t, key[*key_pos]);

	while(t)
	{
		pos = search_node(t, key, key_len, key_pos);
		if(pos == t->skey_len)		//match all chars in node
		{
			if(*key_pos < key_len)		//but there are remain chars in the key
			{
				if(t->branches)		//there are still childrens, go on...
				{
					if(is_rt_valid(t))
						p_valid = t;
					p = t;
					t = search_branch(t, key[*key_pos]);
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
M_rt_stub*	M_rt_insert(M_rt_stub** root, M_rt_stub* insert_node, M_rt_arg* arg/*, get_key_t get_key*/)
{
	M_rt_stub* t = *root;
	M_rt_stub* p = NULL;		//parent of t
	
	M_sint32 key_pos = 0;		//pos for insert_node
	M_sint32 pos = 0;			//pos for node in radix tree
	M_sint32 skey_len = insert_node->skey_len;
	M_sint8* skey = insert_node->skey;

	if(!t)
	{		
		*root = insert_node;
		return NULL;
	}

	if(!t->skey)		// only root can has empty string key, it must be invaild node in this case
	{
		p = t;
		if( !(t = search_branch(t, skey[key_pos])) )	//case 0.0)
			t = p;
	}

	while(t)
	{
		pos = search_node(t, skey, skey_len, &key_pos);
		if(pos == t->skey_len)		//match all chars in node
		{
			if(key_pos < skey_len)		//but there are remain chars in the key
			{
				if(t->branches)		//there are still childrens, go on...
				{
					p = t;
					t = search_branch(t, skey[key_pos]);	//if t is NULL, case 2.1)
				}
				else		//no childrens. insert node will be t's child, case 2.1)
					break;
			}
			else	//match successful
			{
				if(is_rt_valid(t))	//caes 1.2)
					return t;
				else				//case 1.1)
				{
					replace_rt_node(t, insert_node);
					update_rt_parent(insert_node, root);
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
		add_rt_node(t, insert_node, insert_node->skey + key_pos, insert_node->skey_len - key_pos);
		update_rt_parent(insert_node, root);
	}
	else
	{
		if(key_pos == skey_len)	// insert node totally matches, case 3.1)
		{
			cut_and_add_rt_node(t, insert_node, root, insert_node->skey + key_pos - pos, pos);
			add_rt_node(insert_node, t, t->skey + pos, t->skey_len - pos);
	}
		else	// both are not partly matches, case 4.1)
		{
			memset(arg->dummy_node, 0, sizeof(M_rt_stub));
			M_b8_init(&arg->dummy_node->flag);
			set_rt_invalid(arg->dummy_node);
			cut_and_add_rt_node(t, arg->dummy_node, root, t->skey, pos);
			add_rt_node(arg->dummy_node, t, t->skey + pos, t->skey_len - pos);
			add_rt_node(arg->dummy_node, insert_node, insert_node->skey + key_pos, insert_node->skey_len - key_pos);
			//update_rt_children(arg->dummy_node);	//is not necessary here
			update_rt_parent(t, root);

			arg->dummy_node = NULL;
		}
	}

	return NULL;
}

M_rt_stub*	M_rt_remove(M_rt_stub** root, M_sint8* key, M_sint32 key_len, M_rt_arg* arg)
{
	M_sint32 key_pos;
	M_rt_stub* remove_node = M_rt_search(*root, key, key_len, RT_MODE_EXACT, &key_pos);

	return remove_node ? M_rt_removenode(root, remove_node, arg) : NULL;
}

M_rt_stub*	M_rt_removenode(M_rt_stub** root, M_rt_stub* remove_node, M_rt_arg* arg)
{
	M_rt_stub* tmp = NULL;
	M_rt_stub* p = remove_node->parent;

	if(!remove_node->branches)//rm is leaf
	{
		if(p)//if it has parent
		{
			rbt_remove_node(&p->branches, &remove_node->branch_stub, get_rbcolor_rt, set_rbcolor_rt);
			if(is_rt_valid(p) || bst_get_node_count_for_rt_tree(p->branches) > 1)	//case 2.1), invalid node must has more than 1 child
			{
				update_rt_parent(remove_node, root);
			}
			else	//case 2.2). node must be invalid, and has just 2 children
			{
				tmp = get_rt_node(p->branches);
				cut_and_add_rt_node(p, tmp, root, tmp->skey - p->skey_len, tmp->skey_len + p->skey_len);
				//update_rt_children(p->branches);	//is not necessary here

				arg->extra_node = p;
			}
		}
		else
			*root = NULL;
	}
	else
	{
		if(bst_get_node_count_for_rt_tree(remove_node->branches) == 1)	// case 3.
		{
			tmp = get_rt_node(remove_node->branches);
			cut_and_add_rt_node(remove_node, tmp, root, tmp->skey - remove_node->skey_len, tmp->skey_len + remove_node->skey_len);
			update_rt_children(remove_node->parent); 
		}
		else	// case 4.
		{
			M_b8_init(&arg->dummy_node->flag);
			set_rt_invalid(arg->dummy_node);
			replace_rt_node(remove_node, arg->dummy_node);
			//following sequence is essential
			update_rt_children(arg->dummy_node);
			update_rt_parent(get_rt_node(arg->dummy_node->branches), root);
			update_rt_parent(arg->dummy_node, root);

			arg->dummy_node = NULL;
		}
	}

	return remove_node;

}

static INLINE void	add_to_free_node_list(M_bst_stub* bst_stub, M_rt_stub** rt_list_last, M_bst_stub** bst_list_last)
{
	(*rt_list_last)->parent = get_rt_node(bst_stub);
	*rt_list_last = (*rt_list_last)->parent;

	//printf("%s(%d) insert into rt_node list\n", (*rt_list_last)->skey, (*rt_list_last)->skey_len);

	(*bst_list_last)->parent = bst_stub;
	(*bst_list_last) = (*bst_list_last)->parent;
}

void		M_rt_freeall(M_rt_stub** root, M_free_t free_node, void* pool)
{
	M_rt_stub* head = *root;
	M_rt_stub* last = head;
	M_rt_stub* tmp;
	M_bst_stub* bst_head;
	M_bst_stub* bst_last;

	//if(last)
	//	printf("%s(%d) insert into rt_node list\n", last->skey, last->skey_len);
	
	while(head)
	{
		bst_head = head->branches;
		bst_last = bst_head;

		if(bst_head)
		{
			add_to_free_node_list(bst_head, &last, &bst_last);

			while(bst_head)
			{
				if(bst_head->left)
					add_to_free_node_list(bst_head->left, &last, &bst_last);
				if(bst_head->right)
					add_to_free_node_list(bst_head->right, &last, &bst_last);

				bst_last->parent = NULL;
				bst_head = bst_head->parent;
			}
		}

		last->parent = NULL;

		tmp = head;
		head = head->parent;
		if(free_node)
			free_node(tmp, pool);
	}

	*root = NULL;
}

M_sint32	M_rt_isvalid(M_rt_stub* node)
{
	return is_rt_valid(node);
}