#include <string.h>
#include <stdlib.h>
#include "M_radix_tree_array.h"

///////////////////////////////////////////////////////////////
// type specific part
///////////////////////////////////////////////////////////////
//int cmp_rt_key_char(char* key1, char* key2);
static int search_rt_node_char(st_rt_node* node, sstring_t* key, int* key_pos);
static void set_rt_array_branch_char_parent(st_rt_node* rt_node, st_array_branch* branch);
static unsigned char merge_rt_node_array_branch_char(st_rt_node* rt_node);
static st_array_branch* create_rt_array_branch_char(st_rt_node* rt_node_array);
static st_rt_node* search_rt_array_branch_char(st_array_branch* node, char c);
static st_rt_node* insert_rt_array_branch_char(st_array_branch* node, char c);
static st_rt_node* remove_rt_array_branch_char(st_array_branch* node, char c, int* count);

static unsigned char is_mergable(st_rt_node* rt_node);

//int cmp_rt_key_char(char* key1, char* key2)
//{
//	return *key1 - *key2;
//}

int search_rt_node_char(st_rt_node* node, sstring_t* key, int* key_pos)
{
	char* k1 = node->key.string;
	char* k2 = key->string + *key_pos;
	int i = 0;
	while(i<key->length && i<node->key.length)
	{
		if(*k1 == *k2)
		{
			k1++;
			k2++;
			i++;
		}
		else
			break;
	}
	*key_pos += i;
	return i;
}

void set_rt_array_branch_char_parent(st_rt_node* rt_node, st_array_branch* branch)
{
	int i = 0;
	for(i = 0; i<branch->length; i++)
		branch->array[i]->parent = rt_node;
}

unsigned char merge_rt_node_array_branch_char(st_rt_node* rt_node)
{
	st_array_branch* b = (st_array_branch*)rt_node->branches;
	st_array_branch* b_tmp = NULL;
	rt_key_t* tmp;
	int merged_length = rt_node->key.length;
	int i = 0;
	if(!b)
		return 1;
	
	if(b->length == 1)
	{
		merged_length += b->array[0]->key.length;
		tmp = malloc_d(sizeof(rt_key_t)*merged_length);

		for(i = 0; i<rt_node->key.length; i++)
			tmp[i] = rt_node->key.string[i];

		for(; i<merged_length; i++)
			tmp[i] = b->array[0]->key.string[i-rt_node->key.length];

		//if(b->length)
		{
			b_tmp = b->array[0]->branches;
			if(b_tmp)
				set_rt_array_branch_char_parent(rt_node, b_tmp);
			free_d(b->array[0]->key.string);
			free_d(b->array);
			free_d(b);
			rt_node->branches = b_tmp;
		}

		free_d(rt_node->key.string);
		rt_node->key.string = tmp;
		rt_node->key.length = merged_length;

		return 1;
	}
	return 0;
}

st_array_branch* create_rt_array_branch_char(st_rt_node* rt_node)
{
	st_array_branch* tmp = malloc_d(sizeof(st_array_branch));
	tmp->length = 1;
	tmp->space = 1;
	tmp->array = malloc_d(sizeof(void*));
	tmp->array[0] = rt_node;

	return tmp;
}

st_rt_node* search_rt_array_branch_char(st_array_branch* node, char c)
{
	short i = 0;
	short j = node->length - 1;//can't minus 1 here, for the searching algorithm used below
	short q = (i+j)>>1;
	int t;
	while(i < j)
	{
		t = c - node->array[q]->key.string[0]; 
		if(t < 0)
			j = q - 1;
		else if(t > 0)
			i = q + 1;
		else
			//return ((st_array_branch*)(node->array[q]->branches))->array[q];
			return node->array[q];

		q = (i+j)>>1;
	}
	if(node->array[q]->key.string[0] == c)
		//return ((st_array_branch*)(node->array[q]->branches))->array[q];
		return node->array[q];
	else
		return NULL;
}

st_rt_node* insert_rt_array_branch_char(st_array_branch* node, char c)
{
	short i = 0;
	short j = node->length - 1;
	short q = (i+j)>>1;	
	st_rt_node** tmp = NULL;
	int t;

	st_rt_node **src, **dst;
	
	while(i < j)
	{
		t = c - node->array[q]->key.string[0]; 
		if(t < 0)
			j = q - 1;
		else if(t > 0)
			i = q + 1;
		else
			//return ((st_array_branch*)(node->array[q]->branches))->array[q];
			return node->array[q];

		q = (i+j)>>1;
	}
	if(q >= 0)
	{
		if(node->array[q]->key.string[0] == c)
			//return ((st_array_branch*)(node->array[q]->branches))->array[q];
			return node->array[q];
	}

	//i == j now, we must determine where to insert the new node pointer
	if(i < q)
		--q;
	//else if(i > q)
	//	q = q;
	else if(i == q && node->array[i]->key.string[0] < c)
		++q;

	//we will insert the new node pointer at position q
	//expand the pointer array if necessary
	if(node->length == node->space)
	{
		if(!node->space)
			node->space = 1;
		else
			node->space <<= 1;

		tmp = (st_rt_node**)malloc_d(sizeof(void*) * node->space);
		src = node->array;
		dst = tmp;
		
		i = 0;
		while(i++ < q)
			*dst++ = *src++;
		
		dst = tmp + node->length;
		src = node->array + node->length - 1;
	}
	else
	{
		dst = node->array + node->length;
		src = dst - 1;
	}

	i = node->length++;
	while(i-- > q)
		*dst-- = *src--;

	*dst = malloc_d(sizeof(st_rt_node));

	if(tmp)
	{
		free_d(node->array);
		node->array = tmp;
	}
	return *dst;
}

st_rt_node* remove_rt_array_branch_char(st_array_branch* node, char c, int* count)
{
	short i = 0;
	short j = node->length - 1;
	short q = (i+j)>>1;	
	st_rt_node** tmp = NULL;
	st_rt_node* ret = NULL;
	int t;

	st_rt_node **src, **dst;
	
	while(i < j)
	{
		t = c - node->array[q]->key.string[0]; 
		if(t < 0)
			j = q - 1;
		else if(t > 0)
			i = q + 1;
		else
		{
			ret = node->array[q];
			break;
		}
		q = (i+j)>>1;
	}

	if(q >= 0)
	{
		if(node->array[q]->key.string[0] == c)
			ret = node->array[q];
	}

	if(!ret)//does not found
	{
		if(count)
			*count = node->length;
		return NULL;
	}

	if(node->length == 1)//only one child, and it will be removed
	{
		free_d(node->array);
		free_d(node);
		*count = 0;
		return ret;
	}

	//q is the position of the removing node pointer
	//shrink the pointer array if necessary
	if(--node->length < (node->space >> 1))
	{
		node->space >>= 1;
		tmp = (st_rt_node**)malloc_d(sizeof(void*) * node->space);
		src = node->array;
		dst = tmp;
		
		i = 0;
		while(i++ < q)
			*dst++ = *src++;
		
		dst = tmp + node->length-1;
		src = node->array + node->length;
	}
	else
	{
		dst = node->array + q;
		src = dst + 1;
	}

	i = q;
	while(i++ < node->length)
		*dst++ = *src++;

	if(tmp)
	{
		free_d(node->array);
		node->array = tmp;
	}
	if(count)
		*count = node->length;

	return ret;
}


unsigned char is_mergable(st_rt_node* rt_node)
{
	return 0;
}
////////////////////////////////////////////////////////////////////////////////

st_rt_node* rt_search_array_branch_char(st_rt_node* root, sstring_t* key)
{
	return __rt_search(root, key, search_rt_node_char, search_rt_array_branch_char);
}
st_rt_node* rt_insert_array_branch_char(st_rt_node** root, sstring_t* key)
{
	return __rt_insert(root, key, search_rt_node_char, search_rt_array_branch_char, 
		create_rt_array_branch_char, insert_rt_array_branch_char, set_rt_array_branch_char_parent);
}
st_rt_node* rt_remove_array_branch_char(st_rt_node** root, sstring_t* key)
{
	return __rt_remove(root, key, search_rt_node_char, search_rt_array_branch_char, 
		remove_rt_array_branch_char, is_mergable, merge_rt_node_array_branch_char);
}
void rt_free_all_array_branch_char(st_rt_node** root)
{
	st_rt_node* head = *root;
	st_rt_node* last = head;
	st_rt_node* tmp;
	st_array_branch* b;
	int i = 0;
	while(head)
	{
		if(head->branches)
		{
			b = head->branches;
			for(i = 0; i<b->length; i++)
			{
				last->parent = b->array[i];
				last = last->parent;
			}
			last->parent = NULL;
		}
		
		tmp = head->parent;
		if(head->branches)
		{
			free_d(((st_array_branch*)head->branches)->array);
			free_d(head->branches);
		}
		free_d(head->key.string);
		free_d(head);
		head = tmp;
	}

	*root = NULL;
}