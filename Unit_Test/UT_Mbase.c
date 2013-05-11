/* 
	UT_Mbase.h : 测试MBase模块

	kevin.zheng@gmail.com
	2012/09/02
*/

#include <time.h>
#include "UT_MBase.h"
#include "ut_types.h"
#include "pthread.h"

M_sint32	UT_list()
{
	M_sint32 ret = 1;

	UT_CONCLUDE((ret));

	return ret;
}

M_sint32	UT_rbtree()
{
	M_sint32 ret = 1;

	UT_CONCLUDE(ret);
	return ret;
}

struct shash_arg
{
	M_slist slist_stub;
	M_dlist dlist_stub;
	M_bst_stub bst_stub;
	M_sint32  key;
	M_sint32  color;
};

M_size test_hash_key(void* key)
{
	return (M_size)(*(M_sint32*)key);
}

void* test_get_slist_key(void* stub)
{
	return &(container_of(stub, struct shash_arg, slist_stub)->key);
}

void* test_get_dlist_key(void* stub)
{
	return &(container_of(stub, struct shash_arg, dlist_stub)->key);
}

void* test_get_bst_key(void* stub)
{
	return &(container_of(stub, struct shash_arg, bst_stub)->key);
}

int	test_get_rbcolor(void* stub)
{
	return container_of(stub, struct shash_arg, bst_stub)->color;
}

void test_set_rbcolor(void* stub, int color)
{
	container_of(stub, struct shash_arg, bst_stub)->color = color;
}

#define hash_len 100
M_sint32	UT_slisthash()
{
	M_sint32 ret = 1;
	M_sint32 r;

	struct shash_arg ha[hash_len];
	struct shash_arg test_node;
	void* hash_arr[10];
	int i = 0, j;
	M_hash hash;
	M_hash_iter hi1, hi2;
	struct shash_arg* node, *node1;
	M_slist *head, *cursor, *cursor1;

	for(i=0; i<hash_len; i++)
	{
		ha[i].key = i;
	}

	M_hash_construct(&hash);
	hash.init(&hash, (void*)hash_arr, 10, HT_SLIST);
	hash.set_func(&hash, test_hash_key, cmp_key_M_sint32, test_get_slist_key, NULL, NULL);

	M_hash_iter_construct(&hi1);
	M_hash_iter_construct(&hi2);

	hi1.attach(&hi1, &hash);
	hi2.attach(&hi2, &hash);

	for(i=0; i<hash_len; i++)
	{
		r = hash.insearch(&hash, &ha[i].slist_stub);
		ret &= UT_ASSERT(r == 1);
	}

	test_node.key = 33;
	r = hash.insearch(&hash, &test_node.slist_stub);
	ret &= UT_ASSERT(r == 0);

	// 测试每个链表的节点
	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_slist*)hash.ht+i;
		cursor = head->next;
		j = 10;
		while(cursor != head)
		{
			j--;
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
		}

		ret &= UT_ASSERT( j == 0);
	}

	hi2.end(&hi2);
	
	for(i=9; i>=0; --i)
	{
		for(j=0; j<10; j++)
		{
			ret &= UT_ASSERT(hi2.current_index == i);
			cursor = hi2.move_prev(&hi2);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j == 9)
				ret &= UT_ASSERT(hi2.current_index == i-1);
			else
				ret &= UT_ASSERT(hi2.current_index == i);
			
		}
	}

	//测试hash查找
	r = 44;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == r);

	r = 31;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == r);

	r = 99;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == r);

	//测试hash节点删除

	r = 88;
	cursor = hash.remove(&hash, &r);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == r);

	for(i=0; i<r; i++)
	{
		cursor = hash.remove(&hash, &i);
		node = container_of(cursor, struct shash_arg, slist_stub);
		ret &= UT_ASSERT(node->key == i);
	}

	for(i=r+1; i<hash_len; i++)
	{
		cursor = hash.remove(&hash, &i);
		node = container_of(cursor, struct shash_arg, slist_stub);
		ret &= UT_ASSERT(node->key == i);
	}

	for(i=0; i<10; i++)
	{
		head = (M_slist*)hash.ht + i;
		ret &= UT_ASSERT(slist_empty(head));
	}

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hash.insert(&hash, &ha[i].slist_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_slist*)hash.ht+i;
		cursor = head->next;
		j = 10;
		while(cursor != head)
		{
			j--;
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
		}

		ret &= UT_ASSERT( j == 0);
	}

	ret &= UT_ASSERT(hi1.current == NULL);
	
	//从中间remove self
	r = 93;
	hi1.search(&hi1, &r);
	for(i=3; i<10; i++)
	{
		for(j=9; j>=0; j--)
		{
			cursor = hi1.remove_self(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	for(i=2; i>=0; i--)
	{
		for(j=0; j<10; j++)
		{
			cursor = hi1.remove_self(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	ret &= UT_ASSERT(hi1.current == NULL);

	cursor = hi1.remove_self(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=0; i<10; i++)
	{
		head = (M_slist*)hash.ht + i;
		ret &= UT_ASSERT(slist_empty(head));
	}

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hi1.insert_next(&hi1, &ha[i].slist_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_slist*)hash.ht+i;
		cursor = head->next;
		j = 10;
		while(cursor != head)
		{
			j--;
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
		}

		ret &= UT_ASSERT( j == 0);
	}

	//从中间remove next和remove prev
	r = 5;
	hi1.search(&hi1, &r);
	for(i=6; i<10; i++)
	{
		for(j=9; j>=0; j--)
		{
			cursor = hi1.remove_next(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	for(i=6; i<10; i++)
	{
		head = (M_slist*)hash.ht + i;
		ret &= UT_ASSERT(slist_empty(head));
	}

	cursor = hi1.remove_next(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=5; i>=0; i--)
	{
		for(j=0; j<10; j++)
		{
			cursor = hi1.remove_prev(&hi1);
			if(i==5 && j==0)
				++j;
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	cursor = hi1.remove_prev(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=0; i<5; i++)
	{
		head = (M_slist*)hash.ht + i;
		ret &= UT_ASSERT(slist_empty(head));
	}
	head = (M_slist*)hash.ht + 5;
	ret &= UT_ASSERT(head->next == &ha[5].slist_stub && ha[5].slist_stub.next == head);

	ret &= UT_ASSERT(hi1.current == &ha[5].slist_stub);

	cursor = hi1.remove_self(&hi1);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 5 && hi1.current == NULL);

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hi1.insert_prev(&hi1, &ha[i].slist_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_slist*)hash.ht+i;
		cursor = head->next;
		j = 0;
		while(cursor != head)
		{
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, slist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j==9)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
			j++;
		}

		ret &= UT_ASSERT( j == 10);
	}

	r = 55;
	cursor = hi1.search(&hi1, &r);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 55);
	cursor = hi1.remove_self(&hi1);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 55);
	node = container_of(hi1.current, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 65);

	cursor = hi1.remove_next(&hi1);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 75);
	node = container_of(hi1.current, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 65);

	cursor = hi1.remove_prev(&hi1);
	node = container_of(cursor, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 45);
	node = container_of(hi1.current, struct shash_arg, slist_stub);
	ret &= UT_ASSERT(node->key == 65);

	hi1.detach(&hi1);
	hi2.detach(&hi2);

	UT_CONCLUDE(ret);
	return ret;
}

M_sint32	UT_dlisthash()
{
	M_sint32 ret = 1;
	M_sint32 r;

	struct shash_arg ha[hash_len];
	struct shash_arg test_node;
	M_dlist hash_arr[10];
	int i = 0, j;
	M_hash hash;
	M_hash_iter hi1, hi2;
	struct shash_arg* node, *node1;
	M_dlist *head, *cursor, *cursor1;

	for(i=0; i<hash_len; i++)
	{
		ha[i].key = i;
	}

	M_hash_construct(&hash);
	hash.init(&hash, (void*)hash_arr, 10, HT_DLIST);
	hash.set_func(&hash, test_hash_key, cmp_key_M_sint32, test_get_dlist_key, NULL, NULL);

	M_hash_iter_construct(&hi1);
	M_hash_iter_construct(&hi2);

	hi1.attach(&hi1, &hash);
	hi2.attach(&hi2, &hash);

	for(i=0; i<hash_len; i++)
	{
		r = hash.insearch(&hash, &ha[i].dlist_stub);
		ret &= UT_ASSERT(r == 1);
	}

	test_node.key = 33;
	r = hash.insearch(&hash, &test_node.dlist_stub);
	ret &= UT_ASSERT(r == 0);

	// 测试每个链表的节点
	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_dlist*)hash.ht+i;
		cursor = head->next;
		j = 10;
		while(cursor != head)
		{
			j--;
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node1 = container_of(cursor1, struct shash_arg, dlist_stub);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
		}

		ret &= UT_ASSERT( j == 0);
	}

	hi2.end(&hi2);
	
	for(i=9; i>=0; --i)
	{
		for(j=0; j<10; j++)
		{
			ret &= UT_ASSERT(hi2.current_index == i);
			cursor = hi2.move_prev(&hi2);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j == 9)
				ret &= UT_ASSERT(hi2.current_index == i-1);
			else
				ret &= UT_ASSERT(hi2.current_index == i);
			
		}
	}

	//测试hash查找
	r = 44;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == r);

	r = 31;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == r);

	r = 99;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == r);

	//测试hash节点删除

	r = 88;
	cursor = hash.remove(&hash, &r);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == r);

	for(i=0; i<r; i++)
	{
		cursor = hash.remove(&hash, &i);
		node = container_of(cursor, struct shash_arg, dlist_stub);
		ret &= UT_ASSERT(node->key == i);
	}

	for(i=r+1; i<hash_len; i++)
	{
		cursor = hash.remove(&hash, &i);
		node = container_of(cursor, struct shash_arg, dlist_stub);
		ret &= UT_ASSERT(node->key == i);
	}

	for(i=0; i<10; i++)
	{
		head = (M_dlist*)hash.ht + i;
		ret &= UT_ASSERT(dlist_empty(head));
	}

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hash.insert(&hash, &ha[i].dlist_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_dlist*)hash.ht+i;
		cursor = head->next;
		j = 10;
		while(cursor != head)
		{
			j--;
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
		}

		ret &= UT_ASSERT( j == 0);
	}

	ret &= UT_ASSERT(hi1.current == NULL);
	
	//从中间remove self
	r = 93;
	hi1.search(&hi1, &r);
	for(i=3; i<10; i++)
	{
		for(j=9; j>=0; j--)
		{
			cursor = hi1.remove_self(&hi1);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	for(i=2; i>=0; i--)
	{
		for(j=0; j<10; j++)
		{
			cursor = hi1.remove_self(&hi1);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	ret &= UT_ASSERT(hi1.current == NULL);

	cursor = hi1.remove_self(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=0; i<10; i++)
	{
		head = (M_dlist*)hash.ht + i;
		ret &= UT_ASSERT(dlist_empty(head));
	}

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hi1.insert_next(&hi1, &ha[i].dlist_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_dlist*)hash.ht+i;
		cursor = head->next;
		j = 10;
		while(cursor != head)
		{
			j--;
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
		}

		ret &= UT_ASSERT( j == 0);
	}

	//从中间remove next和remove prev
	r = 5;
	hi1.search(&hi1, &r);
	for(i=6; i<10; i++)
	{
		for(j=9; j>=0; j--)
		{
			cursor = hi1.remove_next(&hi1);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	for(i=6; i<10; i++)
	{
		head = (M_dlist*)hash.ht + i;
		ret &= UT_ASSERT(dlist_empty(head));
	}

	cursor = hi1.remove_next(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=5; i>=0; i--)
	{
		for(j=0; j<10; j++)
		{
			cursor = hi1.remove_prev(&hi1);
			if(i==5 && j==0)
				++j;
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	cursor = hi1.remove_prev(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=0; i<5; i++)
	{
		head = (M_dlist*)hash.ht + i;
		ret &= UT_ASSERT(dlist_empty(head));
	}
	head = (M_dlist*)hash.ht + 5;
	ret &= UT_ASSERT(head->next == &ha[5].dlist_stub && ha[5].dlist_stub.next == head);

	ret &= UT_ASSERT(hi1.current == &ha[5].dlist_stub);

	cursor = hi1.remove_self(&hi1);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 5 && hi1.current == NULL);

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hi1.insert_prev(&hi1, &ha[i].dlist_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_dlist*)hash.ht+i;
		cursor = head->next;
		j = 0;
		while(cursor != head)
		{
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node = container_of(cursor, struct shash_arg, dlist_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = cursor->next;
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j==9)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
			j++;
		}

		ret &= UT_ASSERT( j == 10);
	}

	r = 55;
	cursor = hi1.search(&hi1, &r);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 55);
	cursor = hi1.remove_self(&hi1);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 55);
	node = container_of(hi1.current, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 65);

	cursor = hi1.remove_next(&hi1);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 75);
	node = container_of(hi1.current, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 65);

	cursor = hi1.remove_prev(&hi1);
	node = container_of(cursor, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 45);
	node = container_of(hi1.current, struct shash_arg, dlist_stub);
	ret &= UT_ASSERT(node->key == 65);

	hi1.detach(&hi1);
	hi2.detach(&hi2);

	UT_CONCLUDE(ret);
	return ret;
}


M_sint32	UT_bsthash()
{
	M_sint32 ret = 1;
	M_sint32 r;

	struct shash_arg ha[hash_len];
	struct shash_arg test_node;
	M_bst_stub* hash_arr[10];
	int i = 0, j;
	M_hash hash;
	M_hash_iter hi1, hi2;
	struct shash_arg* node, *node1;
	M_bst_stub **head, *cursor, *cursor1;

	for(i=0; i<hash_len; i++)
	{
		ha[i].key = i;
	}

	M_hash_construct(&hash);
	hash.init(&hash, hash_arr, 10, HT_RBTREE);
	hash.set_func(&hash, test_hash_key, cmp_key_M_sint32, test_get_bst_key, test_get_rbcolor, test_set_rbcolor);

	M_hash_iter_construct(&hi1);
	M_hash_iter_construct(&hi2);

	hi1.attach(&hi1, &hash);
	hi2.attach(&hi2, &hash);

	for(i=0; i<hash_len; i++)
	{
		r = hash.insearch(&hash, &ha[i].bst_stub);
		ret &= UT_ASSERT(r == 1);
	}

	test_node.key = 33;
	r = hash.insearch(&hash, &test_node.bst_stub);
	ret &= UT_ASSERT(r == 0);

	// 测试每个链表的节点
	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht+i;
		cursor = bst_get_first(*head);
		j = 0;
		while(cursor != NULL)
		{
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node1 = container_of(cursor1, struct shash_arg, bst_stub);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = bst_successor(cursor);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j == 9)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
			j++;
		}

		ret &= UT_ASSERT( j == 10);
	}

	hi2.end(&hi2);
	
	for(i=9; i>=0; --i)
	{
		for(j=9; j>=0; j--)
		{
			ret &= UT_ASSERT(hi2.current_index == i);
			cursor = hi2.move_prev(&hi2);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(!j)
				ret &= UT_ASSERT(hi2.current_index == i-1);
			else
				ret &= UT_ASSERT(hi2.current_index == i);		
		}
	}

	//测试hash查找
	r = 44;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == r);

	r = 31;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == r);

	r = 99;
	cursor = hash.search(&hash, &r);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == r);

	//测试hash节点删除

	r = 88;
	cursor = hash.remove(&hash, &r);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == r);

	for(i=0; i<r; i++)
	{
		cursor = hash.remove(&hash, &i);
		node = container_of(cursor, struct shash_arg, bst_stub);
		ret &= UT_ASSERT(node->key == i);
	}

	for(i=r+1; i<hash_len; i++)
	{
		cursor = hash.remove(&hash, &i);
		node = container_of(cursor, struct shash_arg, bst_stub);
		ret &= UT_ASSERT(node->key == i);
	}

	for(i=0; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht + i;
		ret &= UT_ASSERT(!(*head));
	}

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hash.insert(&hash, &ha[i].bst_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht+i;
		cursor = bst_get_first(*head);
		j = 0;
		while(cursor != NULL)
		{
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node1 = container_of(cursor1, struct shash_arg, bst_stub);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = bst_successor(cursor);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j == 9)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
			j++;
		}

		ret &= UT_ASSERT( j == 10);
	}

	ret &= UT_ASSERT(hi1.current == NULL);
	
	//从中间remove self
	r = 3;
	hi1.search(&hi1, &r);
	for(i=3; i<10; i++)
	{
		for(j=0; j<10; j++)
		{
			cursor = hi1.remove_self(&hi1);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	for(i=2; i>=0; i--)
	{
		for(j=9; j>=0; j--)
		{
			cursor = hi1.remove_self(&hi1);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	ret &= UT_ASSERT(hi1.current == NULL);

	cursor = hi1.remove_self(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=0; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht + i;
		ret &= UT_ASSERT(!(*head));
	}

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hi1.insert_next(&hi1, &ha[i].bst_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht+i;
		cursor = bst_get_first(*head);
		j = 0;
		while(cursor != NULL)
		{
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node1 = container_of(cursor1, struct shash_arg, bst_stub);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = bst_successor(cursor);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j == 9)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
			j++;
		}

		ret &= UT_ASSERT( j == 10);
	}

	//从中间remove next和remove prev
	r = 95;
	hi1.search(&hi1, &r);
	for(i=6; i<10; i++)
	{
		for(j=0; j<10; j++)
		{
			cursor = hi1.remove_next(&hi1);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	for(i=6; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht + i;
		ret &= UT_ASSERT(!(*head));
	}

	cursor = hi1.remove_next(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=5; i>=0; i--)
	{
		for(j=9; j>=0; j--)
		{
			cursor = hi1.remove_prev(&hi1);
			if(i==5 && j==9)
				--j;
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(node->key == j*10 + i);
		}
	}

	cursor = hi1.remove_prev(&hi1);
	ret &= UT_ASSERT(cursor == NULL);

	for(i=0; i<5; i++)
	{
		head = (M_bst_stub**)hash.ht + i;
		ret &= UT_ASSERT(!(*head));
	}
	head = (M_bst_stub*)hash.ht + 5;
	//ret &= UT_ASSERT(head->next == &ha[5].bst_stub && ha[5].bst_stub.next == head);

	ret &= UT_ASSERT(hi1.current == &ha[95].bst_stub);

	cursor = hi1.remove_self(&hi1);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 95 && hi1.current == NULL);

	//接着插满
	for(i=0; i<hash_len; i++)
	{
		hi1.insert_prev(&hi1, &ha[i].bst_stub);
	}

	hi1.begin(&hi1);
	for(i=0; i<10; i++)
	{
		head = (M_bst_stub**)hash.ht+i;
		cursor = bst_get_first(*head);
		j = 0;
		while(cursor != NULL)
		{
			ret &= UT_ASSERT(hi1.current_index == i);
			cursor1 = hi1.move_next(&hi1);
			node1 = container_of(cursor1, struct shash_arg, bst_stub);
			node = container_of(cursor, struct shash_arg, bst_stub);
			ret &= UT_ASSERT(cursor1 == cursor);
			cursor = bst_successor(cursor);
			ret &= UT_ASSERT(node->key == j*10 + i);
			if(j == 9)
				ret &= UT_ASSERT(hi1.current_index == i+1);
			else
				ret &= UT_ASSERT(hi1.current_index == i);
			j++;
		}

		ret &= UT_ASSERT( j == 10);
	}

	r = 55;
	cursor = hi1.search(&hi1, &r);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 55);
	cursor = hi1.remove_self(&hi1);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 55);
	node = container_of(hi1.current, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 65);

	cursor = hi1.remove_next(&hi1);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 75);
	node = container_of(hi1.current, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 65);

	cursor = hi1.remove_prev(&hi1);
	node = container_of(cursor, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 45);
	node = container_of(hi1.current, struct shash_arg, bst_stub);
	ret &= UT_ASSERT(node->key == 65);

	hi1.detach(&hi1);
	hi2.detach(&hi2);

	UT_CONCLUDE(ret);
	return ret;
}

#define T_NUM		4
#define	LOOP_COUNT	10000
static M_atomic s_count;
struct atom_arg
{
	M_atomic*	pcount;
	M_sint32	tid;
	pthread_t	pid;
};

void* atom_thread(void* arg)
{
	int i = 0;
	struct atom_arg* a = (struct atom_arg*)arg;
	
	for(; i<LOOP_COUNT; ++i)
	{
		atomic_add(a->pcount, 3);
		atomic_sub(a->pcount, 2);
		atomic_inc(a->pcount);
		atomic_dec(a->pcount);

		//a->pcount->atom += 3;
		//a->pcount->atom -= 2;
		//a->pcount->atom++;
		//a->pcount->atom--;
	}

	return NULL;
}


M_sint32	UT_atomic()
{
	M_sint32 ret = 1;
	struct atom_arg arg[T_NUM];
	M_sintptr v = 100;
	M_sintptr newv = 22;
	int i = 0;
	clock_t c1, c2;

	for(; i<T_NUM; ++i)
	{
		arg[i].pcount = &s_count;
		arg[i].tid = i;
	}

	atomic_init(&s_count);

	c1 = clock();
	for(i=0; i<T_NUM; ++i)
		pthread_create(&arg[i].pid, NULL, atom_thread, &arg[i]);

	for(i=0; i<T_NUM; ++i)
		pthread_join(arg[i].pid, NULL);

	c2 = clock();

	printf("%d time for %d concurrent threads with %d loop each\n", c2 - c1, T_NUM, LOOP_COUNT);

	ret &= UT_ASSERT(s_count.atom == T_NUM * LOOP_COUNT);

	s_count.atom = -1;
	ret &= UT_ASSERT(atomic_inctest(&s_count));		//0
	ret &= UT_ASSERT(!atomic_inctest(&s_count));	//1
	ret &= UT_ASSERT(atomic_dectest(&s_count));		//0
	ret &= UT_ASSERT(!atomic_dectest(&s_count));	//-1
	ret &= UT_ASSERT(!atomic_addtest(&s_count, 2));	//1
	ret &= UT_ASSERT(!atomic_subtest(&s_count, 3));	//-2
	ret &= UT_ASSERT(atomic_addtest(&s_count, 2));	//0
	ret &= UT_ASSERT(!atomic_addtest(&s_count, 3));	//3
	ret &= UT_ASSERT(atomic_subtest(&s_count, 3));	//0

	s_count.atom = 1;
	v = atomic_xchg_return(&s_count, v);
	ret &= UT_ASSERT(s_count.atom == 100 && v == 1);

	atomic_xchg(&s_count, &v);
	ret &= UT_ASSERT(s_count.atom == 1 && v == 100);

	s_count.atom = 100;
	ret &= UT_ASSERT(atomic_cmpxchg(&s_count, v, newv) == v && s_count.atom == newv);

	s_count.atom = 1;
	ret &= UT_ASSERT(atomic_cmpxchg(&s_count, v, newv) == s_count.atom);

	atomic_destroy(&s_count);

	UT_CONCLUDE(ret);
	return ret;
}


struct node_arg
{
	volatile M_slist stub;
	M_sint32 tmp[10];
	volatile M_sint32 occ;	//lock of producer and consumer. 0 means not in queue, 1 means in queue
	volatile M_sint32 loop;	//loop number
	M_sint32 list_num;		//identify which array the node is in
	volatile M_sint32 e;	//enqueue number
};


struct de_arg
{
	pthread_t pid;
	M_sint32 tid;
	M_atomic* producer_out_flag;
	M_data_exchanger* de;
};

struct lq_arg
{
	pthread_t pid;
	M_sint32 tid;
	M_atomic* producer_out_flag;
	M_lqueue* lq;
};

#define NODE_NUM		100
#define PRODUCER_NUM	3
#define CONSUMER_NUM	3
static struct node_arg nodes[PRODUCER_NUM][NODE_NUM];

void* de_producer(void* arg)
{
	struct de_arg* a = (struct de_arg*)arg;
	int i, j, k;
	clock_t c1, c2;
	struct node_arg* n = nodes[a->tid];

	k = 0;
	printf("producer %d start...\n", a->tid);
	c1 = clock();
	for(j = 0; j<LOOP_COUNT; ++j)
	{
		for(i=0; i<NODE_NUM; ++i)
		{
			while(n[i].occ)
			{
				++k;
				M_sleep(0);
			}

			//++nodes[a->tid][i].e;
			//nodes[a->tid][i].stub.next = NULL;
			n[i].loop = j;
			UT_ASSERT(n[i].occ == 0);
			n[i].occ = 1;
		//	UT_ASSERT(n[i].tmp[0] == 1 && n[i].tmp[2] == 3);
			n[i].tmp[0] = i;
			UT_ASSERT(n[i].occ == 1);
			de_insert(a->de, &(n[i].stub));
			UT_ASSERT(n[i].stub.next != &n[i].stub);
		}
	}
	c2 = clock();

	printf("producer %d end, elaps %d, fail time %d\n", a->tid, c2 - c1, k);
	//(*(a->producer_out_flag))++;
	atomic_inc((M_atomic*)(a->producer_out_flag));
	return NULL;
}

void* de_consumer(void* arg)
{
	struct de_arg* a = (struct de_arg*)arg;
	int cur_pos[PRODUCER_NUM];	// 记录每个数组的下标位置 
	int loop_pos[PRODUCER_NUM];	// 记录loop的次数，即node_arg->e
								// 这两个参数一定递增，不得下降 
	int i = 0;
	int j = 0;
	int k;
	void* m, *l;
	clock_t c1, c2;


	struct node_arg* node;
	M_slist* slist_stub;

	for(i=0; i<PRODUCER_NUM; ++i)
		cur_pos[i] = -1;
	//memset(cur_pos, 0, sizeof(int)*PRODUCER_NUM);
	memset(loop_pos, 0, sizeof(int)*PRODUCER_NUM);

	printf("consumer %d start...\n", a->tid);
	c1 = clock();
	i = 0;
	while(a->producer_out_flag->atom != PRODUCER_NUM || !de_isempty(a->de))
	{
		if( !(slist_stub = de_remove(a->de)) )
		{
			M_sleep(0);
			++i;
			continue;
		}

		k = 0;
		slist_stub = slist_reverse(slist_stub);
		while(slist_stub)
		{
			node = container_of(slist_stub, struct node_arg, stub);
			UT_ASSERT(node->occ == 1);
			++k;
			//UT_ASSERT(node->tmp[2] == 3);
			node->tmp[2] = 100000000 + j++;
			if(node->loop == loop_pos[node->list_num])
				UT_ASSERT(node->e > cur_pos[node->list_num]);
			else
				UT_ASSERT(node->loop > loop_pos[node->list_num]);

			loop_pos[node->list_num] = node->loop;
			cur_pos[node->list_num] = node->e;

			slist_stub = slist_stub->next;
		//	node->stub.next = NULL;
			node->occ = 0;
			l = node;
		}

		m = l;
	}

	c2 = clock();
	printf("consumer %d end, elaps %d, fail time %d\n", a->tid, c2 - c1, i);
	return NULL;
}


M_sint32	UT_data_exchanger()
{
	int ret = 1;
	int i, j;
	M_data_exchanger	de;
	M_atomic flag;

	struct de_arg args[PRODUCER_NUM + CONSUMER_NUM];

	for(j = 0; j<PRODUCER_NUM; ++j)
	{
		for(i = 0; i<NODE_NUM; ++i)
		{
			nodes[j][i].e = i;
			nodes[j][i].tmp[0] = 1;
			nodes[j][i].tmp[1] = 2;
			nodes[j][i].tmp[2] = 3;
			nodes[j][i].tmp[3] = 4;
			nodes[j][i].tmp[4] = 5;
			nodes[j][i].tmp[5] = 6;
			nodes[j][i].tmp[6] = 7;
			nodes[j][i].tmp[7] = 8;
			nodes[j][i].tmp[8] = 9;
			nodes[j][i].tmp[9] = 10;
			nodes[j][i].list_num = j;
			nodes[j][i].loop = 0;
			nodes[j][i].stub.next = NULL;
			nodes[j][i].occ = 0;
		}
	}

	de_init(&de);
	atomic_init(&flag);
	for(i=0; i<PRODUCER_NUM; ++i)
	{
		args[i].de = &de;
		args[i].tid = i;
		args[i].producer_out_flag = &flag;
	}

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
	{
		args[i].de = &de;
		args[i].tid = i - PRODUCER_NUM;
		args[i].producer_out_flag = &flag;
	}

	for(i=0; i<PRODUCER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, de_producer, &args[i]);

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, de_consumer, &args[i]);

	for(i=0; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_join(args[i].pid, NULL);

	printf("data exchanger test over for %d producers, %d consumers, queue length %d, loop %d\n",
		PRODUCER_NUM, CONSUMER_NUM, NODE_NUM, LOOP_COUNT);

	ret &= UT_ASSERT(de_isempty(&de));

	de_destroy(&de);
	atomic_destroy(&flag);

	UT_CONCLUDE(ret);
	return ret;
}

#ifdef CONSUMER_NUM
#undef CONSUMER_NUM
#endif

#define CONSUMER_NUM 1

void* lq_producer(void* arg)
{
	struct lq_arg* a = (struct lq_arg*)arg;
	int i, j, k;
	clock_t c1, c2;
	struct node_arg* n = nodes[a->tid];

	k = 0;
	printf("producer %d start...\n", a->tid);
	c1 = clock();
	for(j = 0; j<LOOP_COUNT; ++j)
	{
		for(i=0; i<NODE_NUM; ++i)
		{
			while(n[i].occ)
			{
				++k;
				M_sleep(0);
			}

			n[i].loop = j;
			UT_ASSERT(n[i].occ == 0);
			n[i].occ = 1;

			n[i].tmp[0] = i;
			UT_ASSERT(n[i].occ == 1);
			lqueue_insert(a->lq, &(n[i].stub));
		}
	}
	c2 = clock();

	printf("producer %d end, elaps %d, fail time %d\n", a->tid, c2 - c1, k);
	atomic_inc((M_atomic*)(a->producer_out_flag));
	return NULL;
}

void* lq_consumer(void* arg)
{
	struct lq_arg* a = (struct lq_arg*)arg;
	int cur_pos[PRODUCER_NUM];	// 记录每个数组的下标位置 
	int loop_pos[PRODUCER_NUM];	// 记录loop的次数，即node_arg->e
								// 这两个参数一定递增，不得下降 
	int i = 0;
	int j = 0;
	int k;
	void* m, *l;
	clock_t c1, c2;

	struct node_arg* node;
	M_slist* slist_stub;

	for(i=0; i<PRODUCER_NUM; ++i)
		cur_pos[i] = -1;
	memset(loop_pos, 0, sizeof(int)*PRODUCER_NUM);

	printf("consumer %d start...\n", a->tid);
	c1 = clock();
	i = 0;
	while(a->producer_out_flag->atom != PRODUCER_NUM || lqueue_getcount(a->lq))
	{
		if( !(slist_stub = lqueue_remove(a->lq)) )
		{
			M_sleep(0);
			++i;
			continue;
		}

		node = container_of(slist_stub, struct node_arg, stub);
		UT_ASSERT(node->occ == 1);
		cur_pos[node->list_num] = cur_pos[node->list_num] + 1 >= NODE_NUM ? 0 : cur_pos[node->list_num] + 1;
		UT_ASSERT(node->e == cur_pos[node->list_num]);

		//node->stub.next = NULL;
		node->occ = 0;
	}

	c2 = clock();
	printf("consumer %d end, elaps %d, fail time %d\n", a->tid, c2 - c1, i);
	return NULL;
}

void* lq_consumer_removelist(void* arg)
{
	struct lq_arg* a = (struct lq_arg*)arg;
	int cur_pos[PRODUCER_NUM];	// 记录每个数组的下标位置 
	int loop_pos[PRODUCER_NUM];	// 记录loop的次数，即node_arg->e
								// 这两个参数一定递增，不得下降 
	int i = 0;
	int j = 0;
	int k;
	void* m, *l;
	clock_t c1, c2;

	struct node_arg* node;
	M_slist* slist_stub;

	for(i=0; i<PRODUCER_NUM; ++i)
		cur_pos[i] = -1;
	memset(loop_pos, 0, sizeof(int)*PRODUCER_NUM);

	printf("consumer %d start...\n", a->tid);
	c1 = clock();
	i = 0;
	while(a->producer_out_flag->atom != PRODUCER_NUM || lqueue_getcount(a->lq))
	{
		if( !(slist_stub = lqueue_remove_list(a->lq)) )
		{
			M_sleep(0);
			++i;
			continue;
		}

		while(slist_stub)
		{
			node = container_of(slist_stub, struct node_arg, stub);
			slist_stub = slist_stub->next;

			UT_ASSERT(node->occ == 1);
			cur_pos[node->list_num] = cur_pos[node->list_num] + 1 >= NODE_NUM ? 0 : cur_pos[node->list_num] + 1;
			UT_ASSERT(node->e == cur_pos[node->list_num]);
			node->occ = 0;
		}
	}

	c2 = clock();
	printf("consumer %d end, elaps %d, fail time %d\n", a->tid, c2 - c1, i);
	return NULL;
}

void* lq_consumer_removelist_bycount(void* arg)
{
	struct lq_arg* a = (struct lq_arg*)arg;
	int cur_pos[PRODUCER_NUM];	// 记录每个数组的下标位置 
	int loop_pos[PRODUCER_NUM];	// 记录loop的次数，即node_arg->e
								// 这两个参数一定递增，不得下降 
	int i = 0;
	int j = 0;
	int k;
	void* m, *l;
	clock_t c1, c2;

	struct node_arg* node;
	M_slist* slist_stub;
	M_sint32 count = 10;

	for(i=0; i<PRODUCER_NUM; ++i)
		cur_pos[i] = -1;
	memset(loop_pos, 0, sizeof(int)*PRODUCER_NUM);

	printf("consumer %d start...\n", a->tid);
	c1 = clock();
	i = 0;
	while(a->producer_out_flag->atom != PRODUCER_NUM || lqueue_getcount(a->lq))
	{
		count = 3;
		if( !(slist_stub = lqueue_remove_list_bycount(a->lq, &count)) )
		{
			M_sleep(0);
			++i;
			continue;
		}

		j = 0;
		while(slist_stub)
		{
			node = container_of(slist_stub, struct node_arg, stub);
			++j;
			slist_stub = slist_stub->next;

			UT_ASSERT(node->occ == 1);
			cur_pos[node->list_num] = cur_pos[node->list_num] + 1 >= NODE_NUM ? 0 : cur_pos[node->list_num] + 1;
			UT_ASSERT(node->e == cur_pos[node->list_num]);
			node->occ = 0;
		}
		UT_ASSERT(j == count);
	}

	c2 = clock();
	printf("consumer %d end, elaps %d, fail time %d\n", a->tid, c2 - c1, i);
	return NULL;
}

M_sint32	UT_lqueue()
{
	int ret = 1;
	int i, j;
	M_lqueue lq;
	M_atomic flag;

	struct lq_arg args[PRODUCER_NUM + CONSUMER_NUM];

	for(j = 0; j<PRODUCER_NUM; ++j)
	{
		for(i = 0; i<NODE_NUM; ++i)
		{
			nodes[j][i].e = i;
			nodes[j][i].tmp[0] = 1;
			nodes[j][i].tmp[1] = 2;
			nodes[j][i].tmp[2] = 3;
			nodes[j][i].tmp[3] = 4;
			nodes[j][i].tmp[4] = 5;
			nodes[j][i].tmp[5] = 6;
			nodes[j][i].tmp[6] = 7;
			nodes[j][i].tmp[7] = 8;
			nodes[j][i].tmp[8] = 9;
			nodes[j][i].tmp[9] = 10;
			nodes[j][i].list_num = j;
			nodes[j][i].loop = 0;
			nodes[j][i].stub.next = NULL;
			nodes[j][i].occ = 0;
		}
	}

	lqueue_init(&lq);
	atomic_init(&flag);
	for(i=0; i<PRODUCER_NUM; ++i)
	{
		args[i].lq = &lq;
		args[i].tid = i;
		args[i].producer_out_flag = &flag;
	}

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
	{
		args[i].lq = &lq;
		args[i].tid = i - PRODUCER_NUM;
		args[i].producer_out_flag = &flag;
	}

	for(i=0; i<PRODUCER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, lq_producer, &args[i]);

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, lq_consumer, &args[i]);

	for(i=0; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_join(args[i].pid, NULL);

	printf("lqueue single remove test over for %d producers, %d consumers, queue length %d, loop %d\n",
		PRODUCER_NUM, CONSUMER_NUM, NODE_NUM, LOOP_COUNT);

	ret &= UT_ASSERT(!lqueue_getcount(&lq));

	printf("\nlqueue test for remove list start...\n");

	atomic_init(&flag);
	for(i=0; i<PRODUCER_NUM; ++i)
	{
		args[i].lq = &lq;
		args[i].tid = i;
		args[i].producer_out_flag = &flag;
	}

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
	{
		args[i].lq = &lq;
		args[i].tid = i - PRODUCER_NUM;
		args[i].producer_out_flag = &flag;
	}

	for(i=0; i<PRODUCER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, lq_producer, &args[i]);

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, lq_consumer_removelist, &args[i]);

	for(i=0; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_join(args[i].pid, NULL);

	printf("lqueue list remove test over for %d producers, %d consumers, queue length %d, loop %d\n",
		PRODUCER_NUM, CONSUMER_NUM, NODE_NUM, LOOP_COUNT);

	ret &= UT_ASSERT(!lqueue_getcount(&lq));

	printf("\nlqueue test for remove list by count start...\n");

	atomic_init(&flag);
	for(i=0; i<PRODUCER_NUM; ++i)
	{
		args[i].lq = &lq;
		args[i].tid = i;
		args[i].producer_out_flag = &flag;
	}

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
	{
		args[i].lq = &lq;
		args[i].tid = i - PRODUCER_NUM;
		args[i].producer_out_flag = &flag;
	}

	for(i=0; i<PRODUCER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, lq_producer, &args[i]);

	for(; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_create(&args[i].pid, NULL, lq_consumer_removelist_bycount, &args[i]);

	for(i=0; i<PRODUCER_NUM + CONSUMER_NUM; ++i)
		pthread_join(args[i].pid, NULL);

	printf("lqueue list remove by count test over for %d producers, %d consumers, queue length %d, loop %d\n",
		PRODUCER_NUM, CONSUMER_NUM, NODE_NUM, LOOP_COUNT);

	ret &= UT_ASSERT(!lqueue_getcount(&lq));

	lqueue_destroy(&lq);
	atomic_destroy(&flag);

	UT_CONCLUDE(ret);
	return ret;
}

#ifdef LOOP_COUNT
#undef LOOP_COUNT
#endif

#define LOOP_COUNT 10000000
struct race_arg
{
	volatile int a1, a2;
};
void* race_consumer(void* arg)
{
	struct race_arg* a = (struct race_arg*)arg;
	int i = 0;

	clock_t c1, c2;

	printf("race consumer start...\n");
	c1 = clock();

	do {
		while(a->a1){};
		++a->a1;
		++i;
	} while(!a->a2);

	if(!a->a1)
	{
		++a->a1;
		++i;
	}

	c2 = clock();

	printf("race consumer elapse %d\n", c2 - c1);
	UT_ASSERT(i == LOOP_COUNT + 1);
}

void* race_producer(void* arg)
{
	struct race_arg* a = (struct race_arg*)arg;
	int i = 0;
	clock_t c1, c2;
	c1 = clock();

	printf("race producer start...\n");
	for(i=0; i<LOOP_COUNT; ++i)
	{
		while(!a->a1) {};
		a->a1 = 0;
	}
	a->a2 = 1;

	c2 = clock();
	printf("race producer elapse %d\n", c2 - c1);
}

M_sint32 UT_race()
{
	M_sint32 ret = 1;
	pthread_t pid1, pid2;
	volatile struct race_arg a;
	a.a1 = a.a2 = 0;
	
	pthread_create(&pid1, NULL, race_producer, &a);
	pthread_create(&pid2, NULL, race_consumer, &a);

	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);

	ret &= UT_ASSERT(a.a1 == 1 && a.a2 == 1);

	printf("single memory slot race test over for 2 competitors, race time %d\n", LOOP_COUNT);

	UT_CONCLUDE(ret);
	return ret;
}

#ifdef PID_NUM
#undef PID_NUM
#endif
#define PID_NUM 4

struct mutex_arg
{
	M_sint32 x;
	M_atomic lock;
};

M_sint32 thread_id = 0;

void* mutex_thread(void* arg)
{
	M_sint32 i = 0;
	struct mutex_arg* a = (struct mutex_arg*)arg;

	clock_t c1, c2;

	printf("mutex thread %d start...\n", M_get_tid());

	c1 = clock();

	for(i=0; i<LOOP_COUNT; i++)
	{
		atomic_lock(&a->lock);
		UT_ASSERT(a->lock.atom == 1);
		a->x++;
		atomic_unlock(&a->lock);
	}

	c2 = clock();

	printf("mutex thread %d stop, %d loop past, in %d time...\n", M_get_tid(), LOOP_COUNT, c2 - c1);

}

M_sint32	UT_mutex()
{
	M_sint32 ret = 1;
	M_sint32 i = 0;

	struct mutex_arg a;

	pthread_t pid[PID_NUM];

	atomic_init(&a.lock);
	a.x = 0;

	for(i=0; i<PID_NUM; i++)
		pthread_create(&pid[i], NULL, mutex_thread, &a);

	for(i=0; i<PID_NUM; i++)
		pthread_join(pid[i], NULL);

	ret &= UT_ASSERT(a.x == PID_NUM*LOOP_COUNT);

	atomic_destroy(&a.lock);

	UT_CONCLUDE(ret);
	return ret;
}

//
//#ifdef LOOP_COUNT
//#undef LOOP_COUNT
//#endif
//
//#define LOOP_COUNT 1000000

void* rdlock_thread(void* arg)
{
	M_sint32 i = 0;
	struct mutex_arg* a = (struct mutex_arg*)arg;

	clock_t c1, c2;

	printf("rdlock thread %d start...\n", M_get_tid());

	c1 = clock();

	for(i=0; i<LOOP_COUNT; i++)
	{
		atomic_rdlock(&a->lock);
		UT_ASSERT(a->lock.atom > 0 && a->lock.atom <= 2);
		atomic_rwunlock(&a->lock);
	}

	c2 = clock();

	printf("rdlock thread %d stop, %d loop past, in %d time...\n", M_get_tid(), LOOP_COUNT, c2 - c1);

}

void* wrlock_thread(void* arg)
{
	M_sint32 i = 0;
	struct mutex_arg* a = (struct mutex_arg*)arg;

	clock_t c1, c2;

	printf("wrlock thread %d start...\n", M_get_tid());

	c1 = clock();

	for(i=0; i<LOOP_COUNT; i++)
	{
		atomic_wrlock(&a->lock);
		UT_ASSERT(a->lock.atom == -1);
		a->x++;
		atomic_rwunlock(&a->lock);
	}

	c2 = clock();

	printf("wrlock thread %d stop, %d loop past, in %d time...\n", M_get_tid(), LOOP_COUNT, c2 - c1);

}

M_sint32	UT_rwlock()
{
	M_sint32 ret = 1;
	M_sint32 i = 0;

	struct mutex_arg a;

	pthread_t pid[PID_NUM];

	atomic_init(&a.lock);
	a.x = 0;

	for(i=0; i<2; i++)
		pthread_create(&pid[i], NULL, rdlock_thread, &a);

	for(; i<PID_NUM; i++)
		pthread_create(&pid[i], NULL, wrlock_thread, &a);

	for(i=0; i<PID_NUM; i++)
		pthread_join(pid[i], NULL);

	ret &= UT_ASSERT(a.x == (PID_NUM-2)*LOOP_COUNT);

	atomic_destroy(&a.lock);

	UT_CONCLUDE(ret);
	return ret;
}

void* sem_thread(void* arg)
{
	M_sint32 i = 0;
	struct mutex_arg* a = (struct mutex_arg*)arg;

	clock_t c1, c2;

	printf("sem thread %d start...\n", M_get_tid());

	c1 = clock();

	for(i=0; i<LOOP_COUNT; i++)
	{
		atomic_sem_wait(&a->lock);
		UT_ASSERT(a->lock.atom <= 3 && a->lock.atom >= 0);
		atomic_sem_post(&a->lock);
	}

	c2 = clock();

	printf("sem thread %d stop, %d loop past, in %d time...\n", M_get_tid(), LOOP_COUNT, c2 - c1);
}

M_sint32	UT_sem()
{
	M_sint32 ret = 1;
	M_sint32 i = 0;

	struct mutex_arg a;

	pthread_t pid[PID_NUM];

	atomic_init(&a.lock);
	atomic_sem_init(&a.lock, 3);
	a.x = 0;

	for(i=0; i<PID_NUM; i++)
		pthread_create(&pid[i], NULL, sem_thread, &a);

	for(i=0; i<PID_NUM; i++)
		pthread_join(pid[i], NULL);

	atomic_destroy(&a.lock);

	UT_CONCLUDE(ret);
	return ret;
}

#ifdef PID_NUM
#undef PID_NUM
#endif
#define PID_NUM 9


#ifdef LOOP_COUNT
#undef LOOP_COUNT
#endif

#define LOOP_COUNT 1000000
#define TLS_COUNT	100

static M_sint32 s_max_tls_count;

struct tls_arg
{
	M_tls_key x;
	free_t	  destroyer;
	M_atomic  deleteflag;
};

void* tls_thread(void* arg)
{
	struct tls_arg* x = (struct tls_arg*)arg;
	M_sint32 i, j, k, ret, v, r;
	M_sint32 value[TLS_COUNT] = {0};
	M_sint32 createflag[TLS_COUNT] = {0};
	M_sint32* p;
	clock_t c1, c2;
	
	srand(clock()+M_get_tid());

	c1 = clock();
	printf("tls thread %d starts...\n", M_get_tid());
	for(i=0; i<LOOP_COUNT*5; i++)
	{
		j=rand() % s_max_tls_count;
		k=rand_int() % (LOOP_COUNT);
		if(!k)
		{
//#ifndef __M_CFG_PTHREAD_TLS
			r = M_tls_delete(x[j].x);
			atomic_inc(&x[j].deleteflag);
			printf("key %d delete at loop %d of thread %d\n", j, i, M_get_tid());
//#endif
		}
		else
		{
			k %= 2;
			if(k)
			{
				p = M_tls_get(x[j].x);
				if(p != value[j])
				{
					if(p)
						UT_ASSERT(p == value[j] || *p == value[j]);
					else
						UT_ASSERT(x[j].deleteflag.atom || createflag[j] == 0);
				}
			}
			else
			{
				if(j % 2)	//not malloc
				{
					v = rand();
					ret = M_tls_set(x[j].x, (void*)v);
					if(ret < 0)
						UT_ASSERT(x[j].deleteflag.atom);
					else
					{
						value[j] = v;
						createflag[j] = i;
					}
				}
				else		//malloc
				{
					p = malloc(sizeof(int));
					*p = rand();
					ret = M_tls_set(x[j].x, p);
					if(ret < 0)
					{
						UT_ASSERT(x[j].deleteflag.atom);
						free(p);
					}
					else
					{
						createflag[j] = i;
						value[j] = *p;
					}
				}
			}
		}
	}

	c2 = clock();
	printf("tls thread %d stops, time consume %d...\n", M_get_tid(), c2 - c1);
}

void* tls_creater(void* arg)
{
	M_sint32 k;
	M_sint32 i, ret;
	struct tls_arg* x = (struct tls_arg*)arg;
	clock_t c1, c2;

	c1 = clock();
	printf("tls creater %d starts...\n", M_get_tid());
	for(i=0; i<LOOP_COUNT*200; i++)
	{
		if(s_max_tls_count >= TLS_COUNT)
			break;
		k=rand_int() % (LOOP_COUNT*5);
		if(!k)
		{
			atomic_init(&x[s_max_tls_count].deleteflag);
			ret = M_tls_create(&x[s_max_tls_count].x, x[s_max_tls_count].destroyer);
			printf("tls %d created!\n", x[s_max_tls_count++].x);
		}
	}

	c2 = clock();
	printf("tls creater %d stops, time consume %d...\n", M_get_tid(), c2 - c1);
	return NULL;
}

M_sint32	UT_tls()
{
	M_sint32 ret = 1;
	M_sint32 i = 0, j=0, k;

	/*
		起9个线程，每个线程5个变量，
		三个int，两个动态分配
	*/
	pthread_t pid[PID_NUM+1];
	
	struct tls_arg x[TLS_COUNT];

	for(i=0; i<TLS_COUNT/2; i++)
	{
		x[2*i].destroyer = free;
		x[2*i+1].destroyer = NULL;
	}

	M_tls_init();

	for(i=0; i<5; i++)
	{
		atomic_init(&x[i].deleteflag);
		M_tls_create(&x[i].x, x[i].destroyer);
	}

	s_max_tls_count = 5;

	for(i=0; i<PID_NUM; i++)
		pthread_create(&pid[i], NULL, tls_thread, x);

//#ifndef __M_CFG_PTHREAD_TLS
	pthread_create(&pid[PID_NUM], NULL, tls_creater, x);
//#endif

	//M_sleep(2);
	//for(i=0; i<PID_NUM-1; i++)
	//{
	//	for(j=i; j<PID_NUM; j++)
	//	{
	//		k=pthread_equal(pid[i], pid[j]);
	//		printf("%d, %d, thread %d, thread %d\n", k, sizeof(pthread_t), i, j);
	//	}
	//}

	for(i=0; i<PID_NUM; i++)
		pthread_join(pid[i], NULL);

//#ifndef __M_CFG_PTHREAD_TLS
	pthread_join(pid[PID_NUM], NULL);
//#endif

	M_tls_destroy();
	UT_CONCLUDE(ret);
	return ret;
}

M_sint32	UT_err()
{
	M_sint32 ret = 1;

	UT_CONCLUDE(ret);
	return ret;
}
