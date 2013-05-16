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

/*
	replace old_node with new_node
*/
static INLINE void	replace_rt_node(M_rt_stub* old_node, M_rt_stub* new_node, M_rt_stub** root, M_sint32 pos)
{
	M_rt_stub* tmp;
	replace_rbt_node(&old_node->branch_stub, &new_node->branch_stub, get_rbcolor_rt, set_rbcolor_rt);
	new_node->parent = old_node->parent;
	new_node->branches = old_node->branches;

	if(is_rt_valid(new_node))
	{
		new_node->skey += new_node->skey_len - old_node->skey_len;
		new_node->skey_len = old_node->skey_len;
	}
	else
	{
		
		new_node->skey = pos ? old_node->skey : NULL; //old_node->skey is temporarily used here, 
													  //it will be replaced to skey of its child in add_rt_leaf soon
		new_node->skey_len = pos;
		new_node->branches = NULL;

		//if(new_node->branches)
		//{
		//	if(!pos)
		//	{
		//		new_node->skey = NULL;
		//		new_node->skey_len = 0;
		//	}
		//	else
		//	{
		//		tmp = get_rt_node(old_node->branches);
		//		new_node->skey = get_rt_node(old_node->branches)->skey - old_node->skey_len;
		//	}

		//}
		//else
		//{
		//	new_node->skey = old_node->skey;		//temporary use, will be replaced to skey of its child in add_rt_leaf soon
		//	new_node->skey_len = old_node->skey_len;
		//}
	}

	if(!new_node->parent)
		*root = new_node;
}

/*
	add leaf node l to parent node p. 
	l must be independent node, skey locates at head of key string, skey_len is the length of total key string
	this function mainly process skey and skey_len of both p and l
	
	p and l meet following 2 conditions:
	1. p->skey is a subset of l->skey, 
	2. l->skey + pos matches p->skey + p->skey_len, 
	   that means l->skey(pos - p->skey_len : pos) matches p->skey(0 : p->skey_len)
*/
static INLINE void add_rt_leaf(M_rt_stub* p, M_rt_stub* l, M_sint32 pos)
{
	l->parent = p;

	l->skey += pos;
	if(!l->branches)
		l->skey_len = (M_sint16)strlen(l->skey);
	else
		l->skey_len -= (M_sint16)pos;

	rbt_insert(&p->branches, &l->branch_stub, cmp_key_rt, get_key_rt, get_rbcolor_rt, set_rbcolor_rt);

	//update skey of parent, if parent is invalid.
	//this operation is necessary if branches is changed
	if(p->skey)
	{
		if(!is_rt_valid(p))
			p->skey = get_rt_node(p->branches)->skey - p->skey_len;
	}
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

	printf("insert %s", insert_node->skey);

	if(!t)
	{		
		*root = insert_node;
		printf(" success insert as root\n");
		return NULL;
	}

	if(!t->skey)		// only root can has empty string key, it must be invaild node in this case
	{
		p = t;
		if( !(t = search_branch(t, skey[key_pos])) )	//case 0.0)
			t = p;
		//if(!t)
		//{
		//	add_rt_leaf(p, insert_node, insert_node->skey_len);
		//	printf(" success insert as root's leaf, root is %s, %d\n", p->skey, p->skey_len);
		//	return NULL;
		//}
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
				{
					printf(" fail\n");
					return t;
				}
				else				//case 1.1)
				{
					replace_rt_node(t, insert_node, root, -1);
					arg->extra_node = t;
					printf(" success, ");
					while(insert_node)
					{
						printf("node is %s, %d, ", insert_node->skey, insert_node->skey_len);
						insert_node = insert_node->parent;
					}
					printf("\n");
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
		add_rt_leaf(t, insert_node, key_pos);
	}
	else
	{
		if(key_pos == skey_len)	// insert node totally matches, case 3.1)
		{
			rbt_remove_node(&(p->branches), &(t->branch_stub), get_rbcolor_rt, set_rbcolor_rt);
			add_rt_leaf(p, insert_node, key_pos - pos);
			add_rt_leaf(insert_node, t, t->skey_len - pos);
		}
		else	// both are not partly matches, case 4.1)
		{
			M_b8_init(&arg->dummy_node->flag);
			set_rt_invalid(arg->dummy_node);
			replace_rt_node(t, arg->dummy_node, root, pos);
			add_rt_leaf(arg->dummy_node, insert_node, key_pos);
			add_rt_leaf(arg->dummy_node, t, pos);

			arg->dummy_node = NULL;
		}
	}
	
	printf(" success, ");
	while(insert_node)
	{
		printf("node is %s, %d, ", insert_node->skey, insert_node->skey_len);
		insert_node = insert_node->parent;
	}
	printf("\n");


	return NULL;
}

M_rt_stub*	M_rt_remove(M_rt_stub** root, M_sint8* key, M_sint32 key_len, M_rt_arg* arg)
{
	M_sint32 key_pos;
	M_rt_stub* remove_node = M_rt_search(*root, key, key_len, RT_MODE_EXACT, &key_pos);
	return M_rt_removenode(root, remove_node, arg);
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
			if(is_rt_valid(p) || bst_get_node_count(p->branches) > 2)	//case 2.1), invalid node must has more than 1 child
			{}															//keep this empty block for easier code reading
			else	//case 2.2). node must be invalid, and has just 2 children
			{
				tmp = get_rt_node(p->branches);
				replace_rt_node(p, tmp, root, -1);
				arg->extra_node = p;

				if(!tmp->parent)
					*root = tmp;
			}
		}
		else
			*root = NULL;
	}
	else
	{
		if(bst_get_node_count(remove_node->branches) == 1)	// case 3.
		{
			rbt_remove_node(&p->branches, &remove_node->branch_stub, get_rbcolor_rt, set_rbcolor_rt);
			add_rt_leaf(p, get_rt_node(remove_node->branches), -remove_node->skey_len);
		}
		else	// case 4.
		{
			M_b8_init(&arg->dummy_node->flag);
			set_rt_invalid(arg->dummy_node);
			replace_rt_node(remove_node, arg->dummy_node, root, -1);
			arg->dummy_node = NULL;
		}
	}

	return remove_node;

}
void		M_rt_freeall(M_rt_stub** root, M_free_t free_node, void* pool)
{
	M_rt_stub* last = *root;
	M_rt_stub* tmp;
	M_bst_stub* bst_head;
	M_bst_stub* bst_last;
	
	while(last)
	{
		bst_head = last->branches;
		bst_last = bst_head;

		while(bst_head)
		{
			if(bst_head->left)
			{
				last->parent = get_rt_node(bst_head->left);
				last = last->parent;

				bst_last->parent = bst_head->left;
				bst_last = bst_last->parent;
			}
			if(bst_head->right)
			{
				last->parent = get_rt_node(bst_head->right);
				last = last->parent;

				bst_last->parent = bst_head->right;
				bst_last = bst_last->parent;
			}
			bst_last->parent = NULL;

			bst_head = bst_head->parent;
		}

		tmp = last;
		last = last->parent;
		if(free_node)
			free_node(tmp, pool);
	}

	*root = NULL;
}