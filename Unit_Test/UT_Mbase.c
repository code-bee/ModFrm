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
		head = (M_bst_stub**)hash.ht+i%10;
		UT_ASSERT(bst_get_node_count_for_rt_tree(*head) == i/10 + 1 > 3 ? i/10+1 : 3);
		UT_ASSERT(bst_get_node_count(*head) == i/10 + 1);
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
		//UT_ASSERT(bst_get_node_count(*head) == 10);
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
		slist_stub = slist_reverse_NULL(slist_stub);
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

/*
	随机生成有CHAR_NUM三种字符，长度为1~STRING_LEN的串STRING_NUM个，构建radix tree，
	进行增删查改操作，然后释放tree，反复执行多次，看看是否有内存泄露
*/

#ifdef LOOP_COUNT
#undef LOOP_COUNT
#endif

#define LOOP_COUNT  100
#define	STRING_NUM	1000
#define STRING_LEN	100
#define CHAR_NUM	10

typedef struct rt_ut_arg
{
	M_sint8*	key;
	M_rt_stub	rt_stub;
} rt_ut_arg_t;

static M_sint32 memory_blocks = 0;
static void* memory_address[STRING_NUM*10];
static M_sint32 memory_index = 0;

void reg_mem_alloc(void* mem)
{
	memory_blocks++;
	memory_address[memory_index++] = mem;
}

void reg_mem_free(void* mem)
{
	M_sint32 i = 0;
	for(i=0; i<memory_index; i++)
	{
		if(memory_address[i] == mem)
		{
			memory_address[i] = NULL;
			break;
		}
	}
	memory_blocks--;
}

void rt_stub_free(M_rt_stub* rt_stub, void* pool)
{
	void* mem = container_of(rt_stub, rt_ut_arg_t, rt_stub);
	pool = pool;
	//printf("freeing %s(%d)\n", rt_stub->skey, rt_stub->skey_len);
	if(rt_valid(rt_stub))
	{
		free(mem);
		reg_mem_free(mem);
	}
	else
	{
		free(rt_stub);
		reg_mem_free(rt_stub);
	}
}

#define FIX 0

M_sint32	UT_radix_tree()
{
	M_sint32 ret = 1;
	M_sint32 i, j, k, l;
	M_sint32 str_len;
	M_sint8  base_ch = '0';
	M_sint8	 (*str)[STRING_LEN+1] = malloc(sizeof(M_sint8)*STRING_NUM*(STRING_LEN+1));
	rt_ut_arg_t**	nodes = malloc(sizeof(rt_ut_arg_t*)*STRING_NUM);
	M_rt_stub* root = NULL;
	M_rt_stub* search_result = NULL;
	M_rt_stub* dup_nodes = NULL;
	M_rt_stub* dup_tmp;
	M_sint32 matched_len = 0;
	M_rt_arg extra_arg;
	clock_t c1, c2;
	FILE* fp = fopen("D:\\radixtree.txt", "w+");
#if (FIX == 1)
	char* s[10] = {
		"1",
		"2",
		"122",
		"21122",
		"210",
		"0020001100",
		"0102201102",
		"1202",
		"011",
		"10",
	};
	//char* s[20] = {
	//	"001230414",
	//	"3",
	//	"332312342",
	//	"2344",
	//	"222",
	//	"3422203",
	//	"10313",
	//	"123411333",
	//	"04440",
	//	"3102431242",
	//	"04",
	//	"103020323",
	//	"2421102220",
	//	"2421121133",
	//	"24322102",
	//	"0043412",
	//	"2030411",
	//	"303430200",
	//	"04400334",
	//	"243221133",
	//};
	//char* s[10] = {
	//	"1221",
	//	"0",
	//	"21",
	//	"21210",
	//	"0000",
	//	"2100201",
	//	"2020001",
	//	"21020011",
	//	"0000",
	//	"200122220",
	//};
	//char* s[10] = {
	//	"1201001",
	//	"0222010021",
	//	"2111",
	//	"21",
	//	"2000002122",
	//	"10",
	//	"1012221022",
	//	"220111010",
	//	"120020112",
	//	"12010",
	//};
	//char* s[10] = {
	//	"1120202",
	//	"111121",
	//	"1",
	//	"2",
	//	"202021",
	//	"2110001221",
	//	"1020",
	//	"22002002",
	//	"0200100021",
	//	"2",
	//};
	//char* s[10] = {
	//	"2011021",
	//	"11",
	//	"2101",
	//	"1112",
	//	"1222222011",
	//	"1020",
	//	"111010",
	//	"211",
	//	"11",
	//	"022"
	//};
	//char* s[10] = {
	//	"1221",
	//	"1211",
	//	"1010112",
	//	"21",
	//	"202",
	//	"010211",
	//	"2",
	//	"0",
	//	"21100",
	//	"21122"
	//};
	//char* s[10] = {"1210",
	//	"121021210",
	//	"011202220",
	//	"220120010",
	//	"222201012",
	//	"220",
	//	"201012",
	//	"1",
	//	"2000000",
	//	"00" };
	//char* s[10] = {
	//	"00222110",
	//	"0"};
#endif

	c1 = clock();
	srand(time(NULL));
	extra_arg.dummy_node = extra_arg.extra_node = NULL;

	for(i = 0; i<LOOP_COUNT; i++)
	{
		printf("%d ", i);
		//构造字符串
#if(FIX == 0)
		fprintf(fp, "start...\n");
#endif
		memory_index = 0;
		//dup_nodes = NULL;
		for(j = 0; j<STRING_NUM; j++)
		{
#if (FIX == 0)
			str_len = rand() % STRING_LEN + 1;
			for(k=0; k<str_len; k++)
				str[j][k] = rand() % CHAR_NUM + base_ch;
				//str[j][k] = base_ch + j;
			str[j][k] = 0;
			fprintf(fp, "%s\n", str[j]);
			fflush(fp);
#else
			sprintf(str[j], "%s", s[j]);
#endif
			nodes[j] = malloc(sizeof(rt_ut_arg_t));
			reg_mem_alloc(nodes[j]);
			nodes[j]->key = (M_sint8*)&(str[j]);
			rt_init_node(&nodes[j]->rt_stub, nodes[j]->key, strlen(nodes[j]->key));
			if(!extra_arg.dummy_node)
			{
				extra_arg.dummy_node = malloc(sizeof(M_rt_stub));
				reg_mem_alloc(extra_arg.dummy_node);
			}
			
			dup_tmp = rt_insert_node(&root, &nodes[j]->rt_stub, &extra_arg);
			if(dup_tmp)
			{
				nodes[j]->rt_stub.parent = dup_nodes;
				dup_nodes = &nodes[j]->rt_stub;
				str[j][0] = 0;
			}
			if(extra_arg.extra_node)
			{
				free(extra_arg.extra_node);
				reg_mem_free(extra_arg.extra_node);
				extra_arg.extra_node = NULL;
			}
			if(!extra_arg.dummy_node)
			{
				extra_arg.dummy_node = malloc(sizeof(M_rt_stub));
				reg_mem_alloc(extra_arg.dummy_node);
			}
#if 0
			printf("start nodes %d: %s...\n", j, nodes[j]->key);
			for(k=0; k<=j; k++)
			{
				if(str[k][0])
				{
					search_result = &(nodes[k]->rt_stub);
					printf("node %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
					while(search_result)
					{
						printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
						search_result = search_result->parent;
					}
					printf("\n");
				}
			}

			for(k=0; k<=j; k++)
			{
				if(str[k][0])
				{
					search_result = rt_search(root, str[k], strlen(str[k]), RT_MODE_EXACT, &matched_len);
					UT_ASSERT(search_result);
					printf("%s search successfully\n", str[k]);
				}
			}
			printf("\n");
#endif

		}
#if 0
		for(k=0; k<STRING_NUM; k++)
		{
			if(str[k][0])
			{
				search_result = &(nodes[k]->rt_stub);
				printf("node %s(%d)", search_result->skey, search_result->skey_len);
				search_result = search_result->parent;
				while(search_result)
				{
					printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
				}
				printf("\n");
			}
		}
		printf("\n");
#endif
		for(k=0; k<STRING_NUM; k++)
		{
			if(str[k][0])
			{
				search_result = rt_search(root, str[k], strlen(str[k]), RT_MODE_EXACT, &matched_len);
				UT_ASSERT(search_result);
			}
		}
		
		j = rand() % STRING_NUM;
		fprintf(fp, "random j is %d\n", j);
		for(k=0; k<j; k++)
		{
			if(str[k][0])
			{
				search_result = rt_remove(&root, str[k], 0, &extra_arg);
				UT_ASSERT(search_result);
				
				//printf("%s(%d) is freeing\n", search_result->skey, search_result->skey_len);
				free(container_of(search_result, rt_ut_arg_t, rt_stub));
				reg_mem_free(container_of(search_result, rt_ut_arg_t, rt_stub));
				
				if(extra_arg.extra_node)
				{
					free(extra_arg.extra_node);
					reg_mem_free(extra_arg.extra_node);
					extra_arg.extra_node = NULL;
				}
				if(!extra_arg.dummy_node)
				{
					extra_arg.dummy_node = malloc(sizeof(M_rt_stub));
					reg_mem_alloc(extra_arg.dummy_node);
				}
#if 0
				for(l=k+1; l<STRING_NUM; l++)
				{
					if(str[l][0])
					{
						search_result = &(nodes[l]->rt_stub);
						printf("node %s(%d)", search_result->skey, search_result->skey_len);
						search_result = search_result->parent;
						while(search_result)
						{
							printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
							search_result = search_result->parent;
						}
						printf("\n");
					}
				}

				for(l=k+1; l<STRING_NUM; l++)
				{
					if(str[l][0])
					{
						search_result = rt_search(root, str[l], strlen(str[l]), RT_MODE_EXACT, &matched_len);
						UT_ASSERT(search_result);
					}
				}
				printf("\n");
#endif
			}
		}

		

		rt_free_all(&root, rt_stub_free, NULL);

		if(extra_arg.dummy_node)
		{
			free(extra_arg.dummy_node);
			reg_mem_free(extra_arg.dummy_node);
			extra_arg.dummy_node = NULL;
		}

		while(dup_nodes)
		{
			dup_tmp = dup_nodes;
			dup_nodes = dup_nodes->parent;
			free(container_of(dup_tmp, rt_ut_arg_t, rt_stub));
			reg_mem_free(container_of(dup_tmp, rt_ut_arg_t, rt_stub));
		}

		UT_ASSERT(memory_blocks == 0);
		//printf("\n");

	}
	c2 = clock();

	printf("\nneed %d ms\n", c2 - c1);

	memory_blocks = 0;
	
	fclose(fp);
	free(str);
	free(nodes);
	UT_CONCLUDE(ret);
	return ret;
}

M_sint32	UT_radix_tree_pool()
{
	M_sint32 ret = 1;
	M_sint32 i, j, k, l;
	M_sint32 str_len;
	M_sint8  base_ch = '0';
	M_sint8	 (*str)[STRING_LEN+1] = malloc(sizeof(M_sint8)*STRING_NUM*(STRING_LEN+1));
	rt_ut_arg_t**	nodes = malloc(sizeof(rt_ut_arg_t*)*STRING_NUM);
	M_rt_stub* root = NULL;
	M_rt_pool pool;
	M_rt_stub* search_result = NULL;
	M_rt_stub* dup_nodes = NULL;
	M_rt_stub* dup_tmp;
	M_sint32 matched_len = 0;
	M_rt_arg extra_arg;
	M_sint32 extra_alloc = 0;
	clock_t c1, c2;
#if (FIX == 1)
	char* s[10] = {
		"1",
		"2",
		"122",
		"21122",
		"210",
		"0020001100",
		"0102201102",
		"1202",
		"011",
		"10",
	};
#endif

	FILE* fp = fopen("D:\\radixtree.txt", "w+");

	rt_init_pool(&pool, (M_sintptr)offset_of(rt_ut_arg_t, rt_stub), 100);

	c1 = clock();
	srand(time(NULL));
	extra_arg.dummy_node = extra_arg.extra_node = NULL;

	for(i = 0; i<LOOP_COUNT; i++)
	{
		printf("%d ", i);
		//构造字符串
#if(FIX == 0)
		fprintf(fp, "start...\n");
#endif
		memory_index = 0;
		//dup_nodes = NULL;
		for(j = 0; j<STRING_NUM; j++)
		{
#if (FIX == 0)
			str_len = rand() % STRING_LEN + 1;
			for(k=0; k<str_len; k++)
				str[j][k] = rand() % CHAR_NUM + base_ch;
				//str[j][k] = base_ch + j;
			str[j][k] = 0;
			fprintf(fp, "%s\n", str[j]);
			fflush(fp);
#else
			sprintf(str[j], "%s", s[j]);
#endif
			nodes[j] = rt_alloc(sizeof(rt_ut_arg_t), &pool);
			reg_mem_alloc(&nodes[j]->rt_stub);
			nodes[j]->key = (M_sint8*)&(str[j]);
			rt_init_node(&nodes[j]->rt_stub, nodes[j]->key, strlen(nodes[j]->key));
			
			if(extra_arg.extra_node)
				reg_mem_free(extra_arg.extra_node);
			if(!extra_arg.dummy_node)
				extra_alloc = 1;
			rt_process_arg(&pool, &extra_arg);
			if(extra_alloc)
				reg_mem_alloc(extra_arg.dummy_node);
			extra_alloc = 0;
			
			dup_tmp = rt_insert_node(&root, &nodes[j]->rt_stub, &extra_arg);
			if(dup_tmp)
			{
				nodes[j]->rt_stub.parent = dup_nodes;
				dup_nodes = &nodes[j]->rt_stub;
				str[j][0] = 0;
			}

			for(l=0; l<memory_index; l++)
			{
				if(memory_address[l] && memory_address[l] != extra_arg.dummy_node)
				{
					//printf("%d: %s(%d)\n", l, ((M_rt_stub*)memory_address[l])->skey, ((M_rt_stub*)memory_address[l])->skey_len);
					UT_ASSERT(((M_rt_stub*)memory_address[l])->skey 
						|| (!((M_rt_stub*)memory_address[l])->skey && !((M_rt_stub*)memory_address[l])->parent));
				}
			}
		}

		if(extra_arg.extra_node)
			reg_mem_free(extra_arg.extra_node);
		if(!extra_arg.dummy_node)
			extra_alloc = 1;
		rt_process_arg(&pool, &extra_arg);
		if(extra_alloc)
			reg_mem_alloc(extra_arg.dummy_node);

		extra_alloc = 0;
#if 0
		for(k=0; k<STRING_NUM; k++)
		{
			if(str[k][0])
			{
				search_result = &(nodes[k]->rt_stub);
				printf("node %s(%d)", search_result->skey, search_result->skey_len);
				search_result = search_result->parent;
				while(search_result)
				{
					printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
				}
				printf("\n");
			}
		}
		printf("\n");
#endif

		for(k=0; k<STRING_NUM; k++)
		{
			if(str[k][0])
			{
				search_result = rt_search(root, str[k], strlen(str[k]), RT_MODE_EXACT, &matched_len);
				UT_ASSERT(search_result);
			}
		}
		
		j = rand() % STRING_NUM;
		//j = STRING_NUM;
		//j = 2;
		fprintf(fp, "random j is %d\n", j);
		for(k=0; k<j; k++)
		{
			for(l=0; l<memory_index; l++)
			{
				if(memory_address[l] && memory_address[l] != extra_arg.dummy_node)
				{
					//printf("%d: %s(%d)\n", l, ((M_rt_stub*)memory_address[l])->skey, ((M_rt_stub*)memory_address[l])->skey_len);
					UT_ASSERT(((M_rt_stub*)memory_address[l])->skey 
						|| (!((M_rt_stub*)memory_address[l])->skey && !((M_rt_stub*)memory_address[l])->parent));
				}
			}

			if(str[k][0])
			{
				search_result = rt_remove(&root, str[k], 0, &extra_arg);
				UT_ASSERT(search_result);
				
				//printf("%s(%d) is freeing\n", search_result->skey, search_result->skey_len);
				reg_mem_free(search_result);
				rt_free(search_result, &pool);
				
				if(extra_arg.extra_node)
					reg_mem_free(extra_arg.extra_node);
				if(!extra_arg.dummy_node)
					extra_alloc = 1;
				rt_process_arg(&pool, &extra_arg);
				if(extra_alloc)
					reg_mem_alloc(extra_arg.dummy_node);
				extra_alloc = 0;

#if 0
				for(l=k+1; l<STRING_NUM; l++)
				{
					if(str[l][0])
					{
						search_result = &(nodes[l]->rt_stub);
						printf("node %s(%d)", search_result->skey, search_result->skey_len);
						search_result = search_result->parent;
						while(search_result)
						{
							printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
							search_result = search_result->parent;
						}
						printf("\n");
					}
				}
				printf("\n");
#endif
			}
		}

		//rt_free_all(&root, rt_stub_free, NULL);
		rt_free_all_p(&root, &pool);

		if(extra_arg.dummy_node)
			reg_mem_free(extra_arg.dummy_node);
		rt_free(extra_arg.dummy_node, &pool);
		extra_arg.dummy_node = NULL;

		while(dup_nodes)
		{
			dup_tmp = dup_nodes;
			dup_nodes = dup_nodes->parent;
			reg_mem_free(dup_tmp);
			rt_free(dup_tmp, &pool);
		}

		//UT_ASSERT(memory_blocks == 0);
		//printf("\n");

	}

	c2 = clock();
	printf("\nneed %d ms\n", c2 - c1);
	
	fclose(fp);
	rt_destroy_pool(&pool);
	free(str);
	free(nodes);

	memory_blocks = 0;
	
	UT_CONCLUDE(ret);
	return ret;
}


#ifdef STRING_NUM
#undef STRING_NUM
#endif
#define STRING_NUM 1000

#ifdef STRING_LEN
#undef STRING_LEN
#endif
#define STRING_LEN 100

#ifdef FIX
#undef FIX
#endif

#define FIX 0

typedef struct rm_ut_arg
{
	M_sint8*	key;
	M_rm_stub	rt_stub;
} rm_ut_arg_t;

M_sint32	UT_radix_multichar_pool()
{
	M_sint32 ret = 1;
	M_sint32 i, j, k, l;
	M_sint32 str_len;
	M_sint8  base_ch = '0';
	M_sint8	 (*str)[2*STRING_LEN+1] = malloc(sizeof(M_sint8)*STRING_NUM*(2*STRING_LEN+1));
	rm_ut_arg_t**	nodes = malloc(sizeof(rm_ut_arg_t*)*STRING_NUM);
	M_rm_root root;
	M_rt_pool pool;
	M_rt_stub* search_result = NULL;
	M_rt_stub* dup_nodes = NULL;
	M_rt_stub* dup_tmp;
	M_sint32 matched_len = 0;
	M_rt_arg extra_arg;
	M_sint32 extra_alloc = 0;
	M_sint8* x[] = {"00", "10", "20", "30"};
	M_sint32 char_num = sizeof(x)/sizeof(char*);
	M_stackpool	tmp_sp;
	M_sint8*	mem_chunk = malloc(1024*1024);
	clock_t c1, c2;
#if (FIX == 1)
	//char* s[10] = {
	//	"002030",
	//	"30203010203020",
	//	"3030203010300010",
	//	"30102030203020302010",
	//	"302020000010103000",
	//	"0000",
	//	"10300020",
	//	"00302020102030",
	//	"00102020",
	//	"20301000",
	//};
	char* s[] = 
	{
		"203000301000300010003000202000200000302000303010003010201020302030302010002030101020",
		"201030302020203030102010301000000030001000001020200030201020000010302030003020102020302000301000",
		"300000300020001000303010002000000010303000100000300020",
		"3020300020100000000030300030301030001000201000000010200010001020003020003030103020001000101030000030100030303020303010302030202030001030202020300030003020300030201000103010",
		"20200020101010201030100000302010102030101030301010103000300010100000201010302020000030201030",
		"000020203010103010302010301000202030103020302030303020001020101000102030000030203010000030003030303020200000102000102020",
		"0020100030101020101000002030202030302010202020000000000030303010300010203000203010000000200010103020200010101020200020201010103020102000",
		"2010100020003030203030003030003000202030002030101020002010103000302000102030",
		"2010202010",
		"1000101010003000103020200020202000301000102010101010202010002010001020302030101000202000203030201000303020103010200000000020001000000030101010202000203000000010301030301030001030203030",
		"00303010300030200000302000100010102010100000102020301020301000102020000020102010",
		"2010203000203010100010303030",
		"20102000302010000020303030300030002030101020201020001020303000301020302010001030102000300000202000101030200030002030100000100020201010202000103020300010301030303020300020101020200030302010",
		"3000202010200030203010203020100030302020000020000020003030002030300020000010002010003020302020300010001030203010202020100030101000201030203000200010201030200020001010101020303000",
		"0020203010300020201020003000001010201000001010201010201030203010201010003000103020002030103030100010002020201020301020302030100010302020200010301030101030303010103000101030",
		"30001020301010003010203000300030002030302010102010203000102030302030200030300020",
		"1000100020303030103030202000201030102020003010301030201010101000201010",
		"201010003030300030001000101010300020002010002020303000303030101000201020003020203020100000001020200020200020201000",
		"20101000303030303030300010203010103030",
		"2010101020102000000010301030300010100030301010102010201010202010101010201010202020201010001020101010",
		"301010",
		"30203010301010302000203020103010103020100030202010201020201030001010300010103000300030302030202000201000002000203000203000302020300030202010101010202000003000203010",
		"3020101020003000201000002010101000301000302000103020203000203030",
		"100020003030",
		"30202030000010201020102010103010300020",
		"10200030302000301000002010203000203010202020103000200020100010000020",
		"1010303000003020201010103010200010203010100020203010001000203000202020200030101030200020000020102020202030000000202000003030302020",
		"301010201010101020003010102030102020001000001010201030201030103010101020",
		"20001030103000202020003030102020101010003020002030303000001030001030303000102020100030203020003030",
		"201000200000303020000010000000303030101020302010102000203020200020203000302010201010000020100000003000",
		"2000300030300030200020203020003000003000000030000010202030101020302030300020103010003020101030100030301020103030",
		"301010303000003000300010202030102010201000000030203000302030202030001010001030202010002030303020",
		"003030103010001010301010301020",
		"10100000000000002030202000303010001020200000",
		"003000101030303020302010001030003020001010103010201030303030101000000020303020002020003030000020202010003020203020003010",
		"100020100030202030001030103000202020301000003010301020303030102000002010301010203020303020301030102030101020101000002030000020002020302010100030300020100030000030101010203030201030202010201020",
		"200030201030203020202030101010200020",
		"203000003020202000001030300030002030102000302000003030000020102000101000000000100000003020001020100030300010",
		"10101030303020303020302010102010200000303000101000303000201020301030101020103030003030203030000020201000100000303010201020103010302020003010202020302010300020300010303000102010101030",
		"3020003030303000003030003020001000200020001020301010000000103010202000101000102000302000203000302000102020201010203010300030201020301010203000200010",
		"2000103020201030303000201020300000301000003000101010200030301010",
		"301020003030000020",
		"302020100030301010202010101020300020103010002000301010101010003010000010201000100010201010201010",
		"2020303020102030200030200000000000300020200000302030200000303000003010303010301030101020202030302000000010102000001030002000303000301010201000303030302020202000101020",
		"3020203010101030002000301020000000203030203010302010301020103010300020201020202030302030301000201020202020003030202000301000100020201000101030103020101030302010",
		"303000002010203010302010301010000030001000",
		"000000103010003020201000103010002010302020201000000020103000101020003020200010200020203000102020203030302010201010201010102000301020201020101020101000203020001010",
		"1010302030303030000020103000300020003000001030202000003030300010103010003000303000201030002010301000000030200020101000000020301000300000203010103000000030303020202030",
		"101020100010200020001020200000100000102030001010201000101000200020101020300000001030003010201000203030200000101030302020300020102000302020200030202020003010200030002000002000202000",
		"30300020302010200020300020000000",
		"3010002000300000100010101020000010200030101000102030200030302010103030200000303020102000200010202010200000203000302020203020000020103030303000100000103000201000000020",
		"2010002030002000101020001000002010003010203030100000200020102000302020201020101000003000300010203000300000100030002020301030202030203020202020201030001020203000",
		"102010302000103000203030200010303010300020202030201030000020300000201020202020203020003020302020201030200010202020201010002000002000301010302030102020203020100000",
		"20002030102010303030003030301000000030303010301030201020102030303020203000000030300020003010301000000020",
		"001010202000303030100030102020203000302030102000303010200010002010000010301000",
		"301000201010101030303030",
		"1020000000203000001000302030103020300030301030003010",
		"10303000102010000000102010200030201020101000202010200020000010300020301030302030002020002000201030000010100020200000100000000000002020300020003020201000300030",
		"10203030000030001000301010301000202020202030200010001030100030300010101010201030202000002010100010201010000010",
		"20000010302020001000303000100000202020203020000010100000",
		"201000302000101000300020301020200030202010202000202030201030101010202030202030001030002020201020103020001000102010301010101010101000102010101030",
		"20302010002000300000203030300000203010103030",
		"30203010103000200000200000102000202030003010302010101030",
		"2010001010103030203020101010303020303020103000200010102020202020003030101020100000001020003030203010003000200020103000102030202010200010200010203030103030001010000000100010",
		"100020300000303020302000300030103030102020001000100020200010302010101030102030202010202010",
		"0000102020300000201000102020003010000000103020",
		"302030203020102030302030100010202020001020001020202010001030201020303030001020203010101030101000201030103010002000200020300010301020000020200030102020103030100020203010000030",
		"30200020300030003020000000103000003010302010300010302000101000",
		"001030002020000000301010302030301030202030301020302020000010101030000030003000202020201030200020002010003020102010202030",
		"300000302030100010303030203010300020200000203010101020300020202030303010300000303010303030001000101030102010003000000000301010202000000030001010102020301000101030101000",
		"20302000102010301030203020101030100000002000301010103000300010001000000020201010102000201030002000103030200020202000100010303030",
		"30100020101030200010301020000010300030000000100030001000303020203000001020103020203030",
		"10202010300020202030101020001010000010301020103030001020002010002010302020202020300020201000002020302030103020002020002020202030301030",
		"203020202020003000000010100030003020000000000020301010",
		"100030303020300010300010202030100020000030302030102010301020200000002030300000202030102000301010102030200000002010300030203010200010001000001030203030302020003020302010002020303000000030103020",
		"002000303030301030301000102020002000301000",
		"3010001000003030202000302000101020302000302000202020202030303000301000001000101030003030203010300000102000300010302000301000203010000000201010202030",
		"30000030101030302020101020102030301030303030203010201010102020301000201000101010200020",
		"100000002030302030302020303010",
		"2020302030202000300020201020303020002030103010003030200030001020200000301020003030202030000000202020003030101010002000200010003000303010201000002030100010200000000000101020",
		"00000010303020002000202000201020302000200030000000003000002000202020101000303030002030101010301010303020003020100030002010200000203010000000002030301020000000302000202030300010",
		"0010003010303010100010002020103020003000202020101030302020200010300020300030003030",
		"1000203000300000202030303030200000001000003010",
		"200020203030203010303000200000201010001020303030002020300010100030003020001000303020201010202000102000202020102020000010300030200000301020203020300010303000203010003010003000003010300020300010",
		"2030202020300030202000102010303000201010302010",
		"10002010303000102030100000302010301020302020101020300030201010201020103030001010301010001030202000000000101030001010102030000000101030200030301020300020003020201010102020303000",
		"1000003000200000202000201020303000101020000010103030302030301000101030103030303030102000102010101000",
		"300020301030100000101030300000102000301020300000300000000000202020203030000030102020001010301000100020200030302020002030100010300030202030201010001030000000101030100020101020002000100000202020",
		"101010101030203020000000102030302020002000101020102010000010203030301030201020301020102020100020302010301000203010300020103010002000100000302030301030202030200010",
		"1030203030103010302010201000100000100010200030101030102020003000003020200030301030301000302000301020103020102020002030000030102020202020101020",
		"3010003030303010102030201030300000201030000000203010001010103030101020100030302000202000102020001000101010003020200030001000202030300000001030302010303000103000303020",
		"103020100020",
		"001010302020201020102000202000101030003010100010202000103010001030101030302030102000100000202000002010100010300020103010",
		"10100030103010103010000010301020000010200010103010302000201020003010003000102000001030000030302010203010303000002000000000202030102010200030100030",
		"1020002000201030200010001010000010201010301010200020303020102010302030002010303020303030000020001020303000200010102020201030202000003000102010000010102020302030000030301020200020201030302030",
		"000010203030103030002010200020303000102030203030102030303020000030200000203000300030001000203000103000001000201010202010200030102030203010202020202010201010103030302000301000203020003020",
		"200000101020100020001020203030103000301010202020100010202010102020001010303010",
		"2020101030101020302030202030101010001020000030203020200020201010000030000030",
		"00302030001030000000103030100020000020201020001000101020300020203000301010201000201010203030000030303010301010202010202030102020302020300010200020200020000010002020100000200030300020002010200010",
		"2000001020002010303000102030302020203000100000202030",
		"1020103000001030300010",
		"3020201020000010303030300020203020103010100000200000100030002030000020102020300010300020201000101000101000101030201000301030",
		"003020302030100000300030301020303030300010000010",
		"300030001010000020301030301000103020302030201030001000003020303020",
		"00302010203030001030301020303020103020000030002020002020303020202000100000103000300030203020000030203010302020301000102020302010203020103020200010103030202020300000201030",
		"300010003020301030",
		"20001010103030001020301030000030003030303000301020002030200010001000200000200030203020302020002000002000300020",
		"1000001000",
		"0000301000100000302010001020002000103030203000300000100020303010002010001010302010000010002000201030201000",
		"0010003030000020203000102020303030202020103030202000302030000020202000201010201020202020201030300020203000002020302020200020201030202000101030101020000020300030001000000010003020201000002020201020",
		"1030101020203030001030001010202010103020103030100000303020300020302020203010102010300010103030201030000000200020103030301010101000202030003010300010000000003010003020202000300000101000",
		"00102000303000",
		"3010001020201020202030300030002010303000301030203010300010000010203020302020002000003020201010203000302000303010203020301000101020101030",
		"00100030103000103030200020302000302020201030203010100010101000000030201010102020000000301000303010201020200010102010102010103020203010100020000020101000202010201000",
		"201030301030102020303030002010000020201000002030003010100030000030002010200000300020101000001010000000101020202020100030203020300020302010100010002000001010300000002010",
		"003020300010202010201010303030102030003030001000302020003020102020302010102030202030102030100020302030000010003030003010201000301030202020001020002000002030000000103030",
		"20001020302020303030103030302010301000103010200030300010003000000020202000003000",
		"20",
		"102010301030302030201010102030103020000010301030103000100030",
		"10302030301030200000300000100010300010202030302030203030302020103020102010300030003010103000100030203000202000002000100010003020",
		"0000102020301000301010203000100030102010301000001020003010303030303010102010103020303020003030201030202000002020000030201010301010101020301020203030002000000020",
		"001020203010301030001000303010",
		"002020002010002010302000000030002020302030200000100010203020103020",
		"30003010103010103030300010",
		"3020302000000010002010303010302000200000103010202020000000001000000020001000300020301030103020102010201000301030101010101030201010202030301010",
		"102010202010303000003030000000202030301000200030100010303000300020002010103010202000202010303000103010302000301020302030300000100000302000000010300030302030103010203010302000302030",
		"1030301010103000201000001000103000001020000030102030002020203000001020002030102020101020001030",
		"3030",
		"20000030002010102000200000000030302030100000001010302010102030102010201030102010202020102010302020",
		"100020202000202030300030",
		"3010102020303030100000103030003020303010101030203030203020100010003020302020101010003010202030000000002010",
		"201010201030200030001000203000301030000010001000301000100020303010003030003030203030001000",
		"10300000302000300030000010002030003030101000003030",
		"00301020003000003030202020201010303030302020003000002030003010002030301010002010001000201000",
		"202030301000302030003030003020300010103000103020201020100000303000300010300010303030003010302010301030102030301000002020002010001000200000202020001010300030100000202000003020302030",
		"30001030301030101000200000102000200030000000",
		"2000002020",
		"0000203020001020203000303000000030201010203030001030302010200010",
		"10203010302010203000302000203000202000103010101000102000003020101010202020001000101010002020102000202010300030001030303000102010003010301010201000300010303010",
		"1000",
		"203020100030202010",
		"00100020003030002030000020002010203030201010103000303030203020302010003030203030301010101020202030002010",
		"20103020201030301030",
		"10100030203010300030003010201000200020003020303030200010100030201030102030000020002020203030001030201020102010001020200030102010003010003010001010202020002020",
		"203010201020100020303030300010101030201020102010102010003030300020302020001030103000100030003000300000203010100010201020301000300010201020301010002020200020001030101010300010001000001030000000",
		"1030303010",
		"2000001010101000001030300030",
		"3000001020200010303020000000303030201030",
		"10002000101010201010202030000010302000100020202000101010301030302030003000201000",
		"100020303020302010003030302030302010100010301000300020101030301010200030202000200020000020102010202020101000103000200000200030",
		"0030203010",
		"202010101030302000201020102010100000201010101000202010003030302030002020100020203030301010301030101010",
		"1000202020201000003020302030001000303010202030002000001000300000102010302020000030300010003030102010102010003030001030002030000000101020301010303030203000002020000020101020101000202000203020",
		"203030000030202000300020202030101020000010202030103010000010300000203010100010103030201020100020101000001020",
		"100010000010302010100010002010303030",
		"30101010201010301000201000203020201000302030103010",
		"00000010100010202020100000201010302010200030003010002020101020102020002030102030201010202000202000301020301020101020300000102000103020303010300030001010300010101030002020200000300010202020",
		"10300000301020002030003010002020203020201030002000300000300020201020300030302000102020103010201030100030002020300010103010000020002020303030301000200000",
		"003020100010202010301000203030001000100010301020",
		"00100010003000300000203010000020102010103010100020100010001010301010103000003010200020200010100030303030303000302030202030103030101020202010302020301010201020001000102000",
		"001010303020203010002000300010001030002020100000301030000010201010201010203000002030300010001030103000001010001030301010001020101030301000001000302000300030",
		"2020303000301030203020301000002030103000200020",
		"30303020203000003000103010002020102030101000100010002020103000000030203010303030001030100030300030101030101000200000",
		"202000202010302000003000303020102010100000303010200030202030103010100000300020301020",
		"00",
		"20201010001020003010100000102020000000202030303020101030102020202020101000000020102030203000000010101000000020202030100030003010201010002010200020",
		"3010203000303030101000001010201020201020102000103010301010102000",
		"001030303020300000202010200020003030103030202010001000001010001020303000002000301020301030103000100010202010102000001030003000101020000020203010000020102010001030201000",
		"10203030200020203030100000202010000020100000",
		"10000030003020302020003000103020302020302020002010202020001010302020302020000020300000200030302030001020100030",
		"201010201000000030",
		"303000",
		"2020202020001020303030103030002020100010201000101020302010102010001020100020003010301010001030002000001010001000200020000010",
		"2030303010103010101000203010002010201000301010100000203000202020203020002030300020002030002000102030101010302020302010000030000020300030003030200020302010202000300030000020301030",
		"3000300030202000003020",
		"2010002010",
		"302010000030003000302030100030300010",
		"10201030101020003020100020101010301010200000201020101020200010203010303030",
		"302020202020",
		"200030201000302020200000201020303010201010202020201030003020103010302020100020000000100020302030100020000010002000003000300000103010301010000000303020103000202010300020",
		"000030301030200010003000200010101000103000201020303010101000102000202020302010301000000000300010",
		"20300020203030100020303000000010201020000030003000002000300000102020102010302010103000002020302010203010103020001030001010000030102000303010200030200020101010002030100020103030",
		"1000000030302020303020001010101010202020003030102030102010101020203000003020202010303010301020303000200030102020203000200010100030103020301020001000101030300020300020302020102030300010003000202020",
		"103030300010101010202010103030203010001010000010302030100000",
		"001030201020200020302000100020100010001020103020000020001020203010203000203010301020003030300020300030203000300010101030001020002010002010200030103010100030101010102030101000003010",
		"0010002000301000000030100030003000003010303030100030102000300000301030003000200010003020102000001020000000002010002020101000300000303020100010003000",
		"100000302030201020000020001020202030301030300030301030101000101010302010300020200030100000102010302030200030003020002030101020301030203020203030001000303010202010101030301020301020",
		"1020200000300000103020001030100030301030101010102030102000303030102030200020203010200020302000301020001020103020202030101000201020303010102000001020",
		"0010302010",
		"200030203010000030303030001020100030101030003000101000000030300020",
		"20001030102020003010302030101030103010003030300020303030101020202000103020303030203010203020103020301010000020203020201020001010203020302000200030103010301020303020103030203000302000203000103020303000",
		"30003030003020300030301030201000",
		"201010101020302010203030202010203000203020302020002030",
		"00102020300020201010301010001000201010100020302010201000201020000010201030001020303030102020300030103020003020202000300030001000002000300020303010302020200020201010200010100020",
		"1010100030103020003010002000003030001020201030003030001010102020",
		"20301020203010303020100030203010301020301010001010302020103010303030300030200020102010000030202000203010203010001020",
		"20200030103000002010301020102030100010001010103020302000302010",
		"303010301030201000303030302010201030001000201000300000300010203020203030003020302020102010103000200030201030200000",
		"003010203010303010202020301000201020001000001000201020301010001020103000301020303000201010202020003000301020102010001020102010003030203030303020101020301000302000100020102000301010201020201000203020",
		"20301030001000102020003000001030100000301020300010301010300030300010103030001030000010101000302000002010102020201000102030100000101000103030",
		"0000202030101020202020202010001000201000301000202020003020200030000000202030302000302000102020201000200010100010102000102020001000302030103030203000",
		"00303030202000303000301010201020100030001000200030102030300000101030103000301000203000302010003020103020003030203020301010203000002030202030001010303020301020101000",
		"002020100030302030002020001030100020203030003010201030002020102000300000202010003000000030003030101010100020001030002000001020300020201010",
		"3000300030103010202010300030101020301000300010101010301000303010202010103020203030101000203030",
		"00301010",
		"3000002030002010202000103000002020303030103030000000",
		"202010201020000020201020103000301010002010102010100000202020203010302010300030201010101020103020103010203000201020001030301000001020100020100010200000201000",
		"0020203030201010301010003030201020100000003020303010002030101000200000200020302030301030203010303020001010101000300030302000303000200020200030100000200030300010000020202010",
		"30000030300020202020103030100030203030001000201030102030101020300030000030100030100000301010102020000010103010301010002010001010000020101000100000200020102030003030103000103030302030302000",
		"2010202020203030003030000010",
		"00202030301020201000102030200010302000301030300030002000103020101030002010003020202030203000302030202010301020000020101020101000103030201030300030101020",
		"00",
		"10001010103030103030100020002000002030100030202010002030201030000030303030201020103010003030001010002000",
		"0010002000002010300020003010203030003010203030302010302000002010001010000030001020301010102000003030301000",
		"20302010201020201010201010203000300010001030002030001020103020202030202010001030302020100030202010300000301010201010201030000000103030202000203030102020302000100020002010000010100020",
		"003010301030100000000030102000300000202020001020001000303010001000202030103030103020",
		"20101000101010000010302010101020003030102030302010002010003000200030103020001010300010002030301010100000301000200000100010",
		"301010300000003000200030303010102020101020200020301000000000002030102010000020101020201010203010100010302020103000001000300020200030201020",
		"10101020100020103020100030300010302010300000101020202030203020000010001010300010100020101020002010001010000000200030000000003000300020003020102010200000302010203020303020002000102030302020202030203020",
		"2020300020301000000010202000001000003000103030",
		"20003020303000301030101030203000101020102000101000202010200010001030303030102000000030101030303030002000003010201030201030201000100010302020303030",
		"20301000002000100000200010101010203020002000002010000020000030102030101030303000100030000010102020302000102010300010201000",
		"20102010200030103000000020000020203000203000100000102020202000002010202000001010202020101020201030002030001030100010200010303010101030100000",
		"1030203010302000103010002030202020303010200020202010001020001020002000203000202020001030303010302000303000100010101000302020301030300000000000301010",
		"303000001010300030302010302000302000002010102010002030100010",
		"103010301010",
		"003010202000003000202000001030203000300020201020201030202020301030102010",
		"302020001020301020202020100030300010003010100020302020000020102000202000302000203030203000001030200000001030203020303000000010300030000020",
		"3010102010301030002020301010001010201010201030101030303020000010001000200000300000102020",
		"203000301020102000300010200020300020300020303010300030302000000030203030203020002010202000101000000020200000202010201030301020101010301030001020101020301000003010201030301030300020200020",
		"3030200000202030103030301010102020301030102030301020203010001030003030201010103000303030303000300000101020301020101020303010300010",
		"100030202030302000003010203010302020302030203030300000200000102020201010000000100000203000",
		"2030203000002020201030303000303000200000003030203000201030200030200030002000300020201000002020100030102000303010301020003010303020300000102000200020103000000020201030",
		"002000001020000010002020203000100030301000203000302010101010101000302010102000302000300010001010002020001000302000101030302020201020102020",
		"20302000001030302010103030100030103000001000000000303030100000002010000000002020301020300020000010102000300010",
		"20001030202000300010101030203000000030103010201030003000003020000000001010202000003000200030102010001010300020100020300000002020201030100020101020302030103020202030000000303000202010000010102000103000",
		"00201030002000102020303020300020001030203010102000103030100010202010001030102000302010001030001020101010201020002030101010101000101030200020002000201010003010100030201030",
		"30002020003000302000002010003020303030102030200010203020300020301000200010100010201010",
		"20100010100010301000101030003030301020202030001030003030002020203000202010301020002030003000302020301030101010202020",
		"301010000030002020001010001030102020001020100010303010",
		"303010103000103000203000000020301000103010301030303030301020100030103000301020301000000010003000203020001020002020000030301000203000",
		"200000003000203020000020002000002010003020300030002010202020101000202030100030002020101030302020302000003010000000001030101030",
		"20002030302000303030203010",
		"2000000020201010303010",
		"100030302020002010002020000020203030101030101010300030202000001010300010201020202030203030303030003010001030100020002010003010302010003000000010102020201010203020203030101020303010",
		"201000303030202000303010202020002030002020100030001030203030303000100000200030301000302030200020000010100030103020302020300020",
		"301010200020200010202030000020103000101030201020303000200010203010001020000010201010",
		"002000201000002030201000303000202010303020302020003020200000100010103020103030203030102030002000103000103000002000001030103030001000",
		"00303010300020003020301030",
		"103000301010301000003000202030100030301000303000100030003020100000000010101030102030202000202010000020000000101010103000100000300000200020001020303030301000201010003000000010001010",
		"0000302020002000300030101030200030203020103030101030302010203020202000100010200030002030201030002030301010103000203020200030003030302000302010000020303000103030303010100030200010001000002010303000",
		"201000202020",
		"00101010001030300010",
		"203010003020003030301010101010302010000020001030201030203000102020200000003030303020303010300010103000000030303030203000",
		"101010200030102030301020000030103030201030100000203010200020100000103030003020",
		"2010103030302000302000101030",
		"200010103020000020001020003000103010102030202000301010102020203010202020301020200020100020303000103000201020001020000020301020101020001020302030100010201020",
		"0020000030202010000000303010203010002010303020203020101000000030100010",
		"202020102010203000301010103020301030200020202010103030102030103030002000302010200020300020100000201010200020102030001030100000201000303030302010101030000030202010201030",
		"20102000001030003030202030000020303010101010103000003010101030301020301020000000301010001020000030200010202000202000203030202010001030200010201010",
		"2000103000003030001030103030100030300030302020100030200010300030102000102030200000001030300020",
		"20103000001030200030102010201030202020000010003030003020102010203010101000001010103010102010002010301020002020303030200000001000202010300010200020001000001000002020001000200010302010003000301030100010",
		"102020303010003020102010001010200010003010103000003010202010003000200000303030",
		"00003020",
		"1000001020003000200020203010301010303020200010300000002010001030300010102010302030300000",
		"002020302030201010202030303020303030200000100020003000101000202020000010101010300030102010202030202020301000202000302030101020000030100010203020300000101030300010300010202020",
		"203000203030203010003020103000301010",
		"103000103020300010303020301030201030200030100030301030001030302020003000003010200000300010100000102030202010200020201020203000003030303020000020",
		"20201030200010",
		"30001020001000002030002000102010003030203020302000001000101020203020003020301010000000202030103000100030202030102020301010103000001010200030000010003020200020201000202000300020102000202020",
		"200020202010101010302010103020001020001020",
		"3030200020101010200010303000101000001010001000100020100010100030103000300000101030100030203000002010000010102000103020103010303010201030201000200010002010301030203010",
		"203000103000300020200020302010302020300010100000200000101000102010001000101010202000301030003030201010001010103000101030102030103000203030203010303030302000300010003010",
		"1030102030300030100010002030001020203020301010202030103010000030001010202020003030001010003020303010001030001000200020",
		"10100000300000302030302030201000003020103010201000101000301000303010102020301010302020200020202030202030302030302030103020001020101030301030000000200000",
		"203000101000103030302010300030301000001010300000301000200010100030001020300000200000101000102020102020000000103000101010302000300020001010102020",
		"0020100030303000",
		"2020302030101030001000200030200030003030001010300030003010101000103010200030300030202010302000201010002030001030201020302010103030203030201020203030200000200010201000301030",
		"2020",
		"2030100030301010001030301030102000102000203010000010002010303030301020300030102010101000103030103030300030",
		"001000003000203010002030101030301020202030201000301030302010302020203000102000100010300020101000301020000030200030202030301010200010202000102010003010101020101010202000",
		"0030301030102020202020",
		"30102030200030102000002010102010301010001010303020",
		"10001020203000302020000000002010301020200010200030202000203010202030203030103000001020303030300020201020200000202000203030001020",
		"000000003020103000300000300000002010100020100030202030302020000030103020303020003000100010202010300020203010101000203010103000301030103020302010",
		"10202030102010200020103030201020200010102010200030101010202000202000203010301000201000201030101020103020100010201030003000201010303010302010003020200000103000",
		"203000001000201010102030000030003020201030102010301000100030301010103030",
		"2030302010103000302000302010002030001030000010102000000000100000100030203000202010203000203000102010200000001020102020300000102020301030000000303000303030300010301020303010301030101020002030",
		"1020002000001020003020300010201010000000102010301010000010303020100020302000",
		"1010302030201010301030003020301000303020302010303020202010200000201000202010001010201020301010100000301010002010002030002020102030103010100010300010302020203030300030003020200030",
		"30000030203020",
		"001000001010002000002020101000101030101030302010203000000030003010103000200010000000001000200000002030103000201010302030102000200010101030003000303000202020000010",
		"001010203020301020000010000000100020102010102030102020001030300010202010101000103000",
		"2020101010200020000000001010001020201020302030",
		"10002010002030103030300020003010001000102020101030203020203010202030001000000000203030002020",
		"200010202000101010300010001000301000000000302000301020002020301020203020201020302030102030101020303010103030302020203000002010203000203010300030201010303000101030303000102020",
		"203030301030202000001010000010203020003010001010200010002030202030303020203030002010102020303000102010300030102020301020301000003020101000000010",
		"3020303000300030202020003010302020102030200010000010300000100020200030303010301000101020200010303020302010200010101010201010300020200030301000302030001020003030000020002020203010302000",
		"102000301020202020002020002010203020003030202010302010001030003020302010202010001030102030303020201010202000302020103020103010301030200030003030300000",
		"201020101010303030300010302010303030000010101000003000002000302030102010002000100000",
		"30202000301030001010202020303000302020303030203010",
		"3000002020302030003030202010200010100020200000103030302020201000101000102010301010201010103000300030303010300010200020301020202030100010003030201020303030301020303000",
		"3010100000100000000020002030201030000000302030001000201030101010203010000020103030202020100020200000000000203030203030000020000020002010303010101020200010202000200010103030102030",
		"1020101000000000201010100020200030101030303020301010100030300030200030200000000000201030302000203020203010303010302020",
		"003000203030102020101030302000200010102000300010103020302020003000203010100000001030",
		"10103010000020100000002010101030300030200030002020302030001020003010201030002010201000103020301000001020301010101000003020201020003020303020100020201000001020103020201010",
		"000000201000300020200030100020003000002010003000202020200020002020101000201020303030301010002000002020301000001020100000102030103030302030300030101020103020300030002000101030303010200030002030300030",
		"001000103010003030203030102030101030102020302030201020002000002000002030303010301020001020302020001020101020203000102010300020001030202000202020103010202000103030",
		"00201030201000303000000010201030103010302020200030101010300020301000102010201030303030302000100020003010201010301000201010101030003030203000002000001020002010101030201000102000303030001030003020",
		"100000101000201010100000002030201030200010001000",
		"0020003000100000301030000030001020003000103030202000002030000000200010101030300000000000301030300020103030101030300030000030300010003030300020",
		"2000303010301000102000203010",
		"10",
		"000000303010002010001020302020202030",
		"1020200020202010200030001030203020203020003020301000000000002010",
		"1020302000100030200000202030203020300010300000103020303010301000001010200030200010200010002000203030001000102010102030002020200030301010002000103010201010300000301020102030302010200020",
		"3020203000000020103000301000301020100000300010",
		"10003030002020102030303000202010000020",
		"303030103000301000000020301010003000303030202000303000203020102010300000301010303000301010200020203020102030000010302000200010100030",
		"300000101020201000302030003030302000101010301030301010203010103000300000300030301010203030100010303020003030303030002030202030101010301020102010",
		"302030100020002010001010303000103030000000200020003010001010202010200010000010100000203010200000200020100020101010100000002020303020100010300000",
		"2000103030301020101030102030001010103020202010202030003000001010202030300020",
		"1010000020000030303000303010201000101000200020",
		"300020203030001000100030102010002020203000301010300000",
		"30202020203000202010202000200010101000100030303030300030103000100010302000001010002000303010101020101020002020001030303000200030200010100030",
		"10102010203010203000203010",
		"30001030101030",
		"0010302000103020003010101020303010000030303000202010203020201010302020201030102020302000202000200030203020102000200000302030000010003000302010003030202020203020302000",
		"100000001010201000001030302020301030200010000030102030202020302010000020101000302000301010100020101010201010103030101020102010102020002030",
		"3000200020",
		"30103030101000203000302000300020303010103020000030102000103030300010201020302010301010103010002020",
		"303020303010001010",
		"302000201020200000203010001030201030102000200010102010300030001000103020",
		"3010002020003030002030300000202010301010302030100000203030001030102010200000001010102020100020000000102010302000300020100020000030203020003020301000101010203010300030",
		"0030201010",
		"10301010301020002030102030003000203010003030",
		"2020201020103020203020201030101000201000002030000030301000300000201010001020001030001020302010102000001010203030003020002000102010201000",
		"0020300010302020201020302030302010101000200020000020301000202000102030200030102010103020100020202010201000101000103020103000100000002000202000",
		"1000103020103010103010301000202020301020303020001000302000003030100030303030203020303000103000002030002000",
		"003000103010001010300030201020102010100030200000000030001000202000201010200000",
		"30000020101010100020203020002000101000001010103020000020303030300030",
		"10100020103030100030203030",
		"0010302030101010101010303000001000000020100030002020303010301000002030000030302010101010301000101000203000301030202000100030100010302030001030001020",
		"1030103010000000101030303000302020001000203000102010103000001000103000100000002030100030002020302030302000301000101010001020303030002020301000002030200000300010003030302000303010",
		"302000002030301030102000102010001010003030101030303000200020200010101010000020303030000010001030103030301020000020201020100030301000301030000010200020201010100020202020102020103030103030",
		"10102000201010100030100030002020203000100000203020200030102000103000302000001010301020200000203000100020",
		"102030100000201010202020202030202030200000200000203000201000303020000030203000002030001020202030303010003020000000000010302010102000203000100030001020101030201000101020",
		"0030000000100000000010202030202020001000303020",
		"10200000100030202000301010100030000020",
		"201030102020301000003010003010003000302030102000201000000010",
		"30002030001030003020101020001000303020102020100000301010002020100000001000303010300020302020000010102010201030201000103020100030102000203010102000202020000000102010000030300030301000102010",
		"3010301020301030202000301000300010303000",
		"1000000030203030001030001010100000003020102030103000101030001030200020000030203010300020301020",
		"0030201020003020001030002020001000300000300010200020103010001030100000000030303030002000103010302010000030",
		"0030203030003000201030101020301020001030201030100030102030300030300020002000303010100010000000302020",
		"0020000020101030201020200010100020200030301000203030102020101030200010101030",
		"20102000",
		"203030001030302020002010100020200000000010203010100020200020203030102030101030",
		"300020302000302030300030101030000020102010003010103010003030003020200020203030001020100010000030101010201010001020001030102000003020101010003030000020303010",
		"00003010",
		"1010102000201010102000102030101030102000302030102020002010303030300010002030201010302000303030",
		"00001020201010103000202000003030101000300000303030002000200000100020001000300000003010201030201020",
		"000010101000203000203030301030302020300030002030201000000020201020203020003020202000200010202020303020203000100010302030000000301020102030002020301030202030301000301030001020101010103030202000303030",
		"203000301010303010003020103000",
		"1010303030202020001030000010202030103000101000102020302010100010203020202030202000003020100000203010303020",
		"001030001010100030200030102030203000303020002010202010000020002010301020303000002030302020",
		"201030000020000020300020",
		"10202010",
		"10303010003030202030202030002000002030202030202030101000203030102010200030302010100020202030001000101020200010301000001020102010200010302010001000002030202030202020203010100010003000103030301000",
		"20200020103000303030302000202010103000002000202000302030200000001020102030200030001010202020302030001020001010001030103020202010002010003020303020200000201030100020",
		"3020300030202020003030203000300020000030201010002030301010203010303020201000303010300000003000202030303010002010202000100000",
		"0020303010302020200030000020101030103030300000103020301000000000103030100010",
		"0020302000202020203030201030300030202010100020302020003000303010002030203020000000303000303010103030001010202000102030300020001020000030100000202010",
		"3000300020100020",
		"3020300000100010201010103030202020101020002010001030200020202010101030200030100010002030103010303030001030302010003000201020201030100030000010202030300030000010300020",
		"202020200030202020202030300030200000300030300030202010102020102010101030103030102000003030000000003000103020102000102020103010",
		"10302030000010100030300030302010101000103010101000100010301000101030302000002020002020201030300020203020101010102030201000200000103020101010303030",
		"202030201020303000200000102030102030001020202020",
		"00",
		"1000100010300000301020102010100000103020002030002020303000002000201000300010301020301030000000200000103010300000100020001000001020",
		"3000",
		"30001030302020202020003000303020102020200030101020100030102000102030002000200030300010303020101000300010100030000020000020202030303030300010302020",
		"20302010101000",
		"10",
		"002010200020101020303020000000201020201010301030101000100000101010000030202000201000203010001030001020302000103000201030101010200000200000203030102030000000202000002030200030",
		"203000203030001010201030101010000010202030202000300020301000203000002020203030303030101000003010303000200020103030003020300020302030103020100020",
		"30003020203020203000201000300020203000303030300010001020001030201000303000003010302000303010002020101020102030201000003010",
		"3020201030301020000000302010302020301020100000200000103000103010100010102010202020201020201010201030300010101030001010",
		"1000300000303020302020303010303030101020301030203010302010202030300000302000300000302010103010201020200030001000000030201030001030",
		"30002000201000202010200000003000103000103030101000302000201030200010001000303030102000203010303010",
		"00",
		"302000100020200010301000201030300010100000002000303000203010200010103000302030100000303010101000300020302000301000002030201020302010002020",
		"001000303030300010200020201000101020103010003010003010101030102010100000303010200030002010302030303030002020200000100010002030201010100010102030001020103020302010001000301020203010002030001030",
		"103020",
		"30000020103010101030303000002010000030303030301010201030001020002030002020201010203010101030301010300010301000201030100030301010200030102000100030",
		"10301030101020000010001010200000202020302030203020103010203020202010103020200020300030003010100010301020102000300030200030000010003010102010302020000020",
		"3020300000003020202010100030200010003030200020301020000030302000001020101030",
		"20",
		"10002020202020100010000000003010303010000000300030100030101000101010203020100030203020203030103000302020202020300030300030300000",
		"301020103020103000101020202020202020303010102030100020102030103030003010300020300020301020102010101020103010301030303030100020101020103010302020100020003020301030303010001000",
		"00201000",
		"00001000302030202010000030102000201010201020200010303000303020101000",
		"1030100020203000000030001030003020203020202020100020003020101010303020000020003030202030300000100030203020303030001020003030203030201010102000101010101020201000003030201000003030",
		"001010",
		"10100010300000203020301000201030300010202030302000302000202000301000303020102030202010303000102020",
		"30003020303030100000002030002000300030303000300020202010103030200020202020200020200020202000301010002000201030202030102020300000103000103000101030300020302020301000002030303020301000300010303010",
		"30103030100010202020302020102030302030101010200000002010203020301030202000101030",
		"201000301010",
		"10002030302020003010100030300020003030203010",
		"1030",
		"30102010103000",
		"00200030102020100010101000002010201000001000101010001020300020203010302030102020000030302020200000200020001000000020103020002010200020001010102000",
		"302010",
		"3010102030203030203000102020300000203000001030301010101000202030300000302020102010200030101020203010200010100020001020002030003010102000",
		"0030300000100000102010102000001000102000100000101000101000201000101010203020201020002010101000001010000000303010302000300010303010200010100030200030303010001020101030303020002020101030",
		"301020303010003020301020003030302020300000303020",
		"0000201020103030303010302030101000102020302030200030203000301020103030302010102010001030202030100010202010201010103030203030303010000030100010300000002020100010203030303000103030102000303020",
		"001030000000203000302010000010103020102010002030203000201030002000",
		"2030202020103010303030302030202030301030202000302030003020003030203000303020302020",
		"302030003030301010303030303010000000003010003010001020103020003010100030201030101020202030100000100010300010101000203030301020000010102000000020302020102030200000003030200030200000",
		"0000",
		"20101000101000102010101010202030203000100000300020102010203010100010203010103020102030201010203010301010101010002010000000103010",
		"1010301020300000303000303030203010002020302020303010100010000020002030",
		"30200010303010300000000030002030201010102020302000203010303020001000002000200010301010001010303010301000302000201010",
		"302020200030302000301010100030102020000000003020301010303010301000002020302010003030300020100000303000100000100030",
		"3000300020000020001020102000201000101030202030001010001010000030100000102030203030101010000020000000201010002000302030200000202020103000101030303010",
		"201020001000201010000000303030300010101030300030301020000010102030",
		"1020003010203030300020000020200030001020102030101020003010000020100000102000002020301030101010203000101010",
		"302000003000202030202000103020100020301030303000102030102000201010302010201020",
		"1000001000003030202000201000003010001030101020101000300010002020100030201010303030203020202010202010302020",
		"3020001000203000200010303000300000102030201030203000300030100030302020303030101030101000003030300000000000200030003000100020302000002010103020003020200030201010201000",
		"00200010101030102030001000300000201030000030001020300030101030200010001030302030101030302000202030303010000020102000",
		"003020100030303020100010300030301020301010203030001000100000202000300010201030100020201010103010200030001010203000",
		"10000020102000",
		"100010203000000030202000301020002010002000201020100030201030201030100020102010200030103000202020200000300000",
		"3010001030202000",
		"0010301020303000100030302000002000200030003030001010000030202030203020002000202020302000101020000030101020001010103020301000302020002010101030302020102010200000202020303020302000102030",
		"00302030302010201010203010102010101030200030003030303030301000001020003010201020001000301000202020103030001000103000002030",
		"2010301010002030301000202030101000001000202030303030302010102010102030303030100010302000003020002030103020301010103030203000302010001000000030101000103020200020203020101020103020202030003000",
		"2000001020303030103030001020201020301020103030202000102010101000003010",
		"30302010002000203010100000102030001000102010301030303010300000002020303010301010200000003030002010200000001010200010203000200030202030202030202000300030302000203000",
		"0010302010203010001000101010002010202020003020100000101000000030002000300000001000003000202000300010302030100020002020002030103000100030101020",
		"10303010201010303030001000003030201030003000101010003000303030201000",
		"302020101030003000103010102010102000302010",
		"101030301000300010103010102020300010201020300000200020000010003020102000301010000000000020101000002000101000301000003030002020102000102010200030003030101020300020102030002030000010201010102030102000",
		"3020300010300020301000103000302030303010002020300020",
		"10303010201020200030002030201030300010003020",
		"00101000000020002000303030003020302020101030303000003010000010102030000030001000202030100000102030202000302030300020200000000020003030300000202000103030001030201010303010000010301030203000303030",
		"000030302020101010301030202030",
		"1030202000100030002020203030203020103010000010102020002000101000000030202030002000201010200010200000003030303010203000302000002000203030102030102000",
		"10200020000030000010000010100020101030002020000020303000100010003010003000103000001030302000301000002000000000201030203010101010201030002030003030300010",
		"1020301020000000300020301030101010302020103020202010302030301000203000301030203000100010102020200010200000201010203000001000002010201020001000002020302030201020",
		"102020001000203030302010202000001020202030203000100020",
		"003030300020300000302010300030302020202020200010101000000020101000301000201000003000001030203030001010203020300030100010102000102020303020303000",
		"000010101020100020002020100030001030202000200020001010201030000020202030301020101030",
		"000020201010301030300000300030300030100020003010301030001000203000301020202020102030002030202030100030000020200030200000201020000010101030003020102000300000",
		"301010300020103010300010301000200020203000003030002010001030",
		"00100010101030103020103010302020303030302030102000003030202030200030101000101010",
		"1000000020003000103000203010100000102000102020000020100010202010103030303010",
		"1030200000201010301030201010202000301000102030",
		"302030301010300020303000302010200030300020000000002030300020001000300010202030000020000030101030",
		"30303020100000100030100030100000202000301010302010303010100000003000101000200020",
		"302010001030",
		"200010202010000020101000100030001020300010201000101000300020201030000020102000303020",
		"000030202030103010003010200030103010202030100000002020302020101020002010203010103010200000203010102020303010101020103000",
		"100020100010",
		"0000101030003020200020103030100020102000302020301030103010000010101020103010202010003010300010203020",
		"1030202010000010302000203010000000201030000030000000100000301010303030102010000030100000002000300000202010302030203020303000001030202010201030203010",
		"30",
		"20103020201020",
		"3030101020302030000020001000003020102020300000100030301030300030300020203010103030200030203000003020100010102010102000103020000030302000202000303010200030101030102000202010",
		"0030202000300020300020001020302010200020203000100020100000301010101000100020001010100010103010102030102020",
		"203010003000102020202000201010001020102000201010",
		"3010303030302020203030103020202030002010200020",
		"20000000101010001000102010200000301020100030303000202020301030303020203020002010203010200020200020001000003020001000003010303030102000103030003000300030300030103030003030201000100010200020",
		"1020002020201030101000",
		"10003030101000000010103010100000100000101000201020103000202010000010002000103030102000102010001020",
		"20303010100010200030003020100030301000001000101020103010101030203010303000000010102020001010102030302030200030300020001020",
		"20001010103010301000203000003030301010102010101010001010100010302030002010000030102020102010303030200010001030202000201020303010303010100000200010202010001030203030101010203000203020",
		"3030202010300000001030",
		"301010102010303010103010301020201000200000",
		"20100010002030002010103000301010002030003030002000003010200010003030003020303010003010302030300020003030101020100020000010103030303010201000200000001030003010000000",
		"000030302020100020",
		"300010101030202000200030200010202000303020101000302000300000302030103030103030003010300000202030102020203000302030",
		"10300020200000301020003000001020300030202030201010201000202020103010303000102010301010002020301010302020103030300020301020301020302030002020202010000030201010",
		"100010103030302020302030202030100020003020303000201020202010101020000030302030101010002000300020003000200030102030202030200010202020000020101000303000202010001010001000203010",
		"0030001000300010303000100000002010203030100000200030300030202010101010302010202020201000102010102010301010203010101000002010003020300030100000103000202010303000001020101000203000102030",
		"30301020301000100010202000303030002030101000103000002010000010101000302020",
		"202020202020000000101020202020303010201030301030200020202030202010002010300010203010100020300010100020103030102020303020002030201020300030",
		"10002000103010002020100030100010301010000020202010102020203000200020301010000010000010101000001000003000000000202020002020000020101000203010301000103020102010300000003000003010101030300010100010",
		"00102000",
		"2030102000303010202000001030001000002030003020103000303030001020200020103020003030300000",
		"2020003000201030001030001020000020202020102020303010302020102010002020200000302030003030002010003010201020000000100000300020303020201010000030",
		"30201000202010300020101000103020103000",
		"0000000000000030203030300000103030300000003010003000300020102030301030001000201000001010001030302000200000201030201000301030201010302020100010302030300010103010001000303020102010003030203000",
		"101000002020001010103030300010100020001020000010202020000000103030001000101030300010003030301030300030201030003020202010003030301030",
		"301020200010302000001030301020101030301020",
		"0030001020",
		"00002030100030101010301020302000001000300000001000201020300000200030201020203000302020001000302000303030102030102000001030001020100010201000303030003010",
		"203030101020301030301000301020002020100030202000201030003030000020201000001010303010101030103020202010101020101030002010101030103010200030",
		"30103030203030302000301000302030001000001010",
		"103030203000303010001020302000003020301000000010001030002010101030002020001030301000",
		"1010301010203000202030202010101000100010200030100010001010202020003030203030100030",
		"0020303010103010",
		"1030302020001010101020003020300030102030203030300020103000203000100000203010000030000010300010003000303010003000003030002010203000",
		"00001000200030201010101030100000",
		"10002020301030201030302030301020200000003000203000202030200000103000201000101010103000003020102020302010103010103020200030001020201000303020300030102020301010103000102000103030001020300030200020303000",
		"102000103020303000102010102020103020200030203010300020302030003010000020303000302020103030001000001010303010002020201020102000100010002010303030000020302000302030",
		"10202000203020103010101030000000001030203030302020001020101030200030102020001010302020",
		"10103020103000101030001000202010",
		"201000000010003010301010300020301030302000300030302000101030202020100010301010202020102020302000201020",
		"001020300000000010100020003000102000201000",
		"3000102000002000302030203010001010",
		"300000103020202000103020103000202030000000200000102010300020",
		"001000103010002030101030103010301000001030001020203000203010303020000000101030101000002000003010303020001010203010302020102000202010101010300030303020",
		"1020001030300000301010300020202030201000002030302020203010102000000020102000002030302020301010103030100010102030100030103020203030103010302000200030003030303010000010",
		"0000200000002030",
		"2000000030003020101030301000203030303030001000101030302020202010201020201010003010301020103000303030103010101010300030003000101000103020302030302020100030201000201030",
		"2010203010201000100020101000303010201010102030100000001030200030300000301010000010101010101000103030101000202020200020200020102000303020201010003010301010",
		"30001030100000303020201020300010200020302020",
		"20002000200020300020103030101020101020100010103020202030102020303000002010",
		"301020103010302000000010001010303010201020000020303020202030201000203000300020002000003020000030",
		"201010301030303000101020101010201030203020202020100020301010100030303010301010102000202020100030001030",
		"00101000202030003030200020002010003030102010",
		"20100010202020001030000010100010000000203000003020201010002030100020001000302010101030000010301030302000302020100030",
		"10103010202000200030201010300020001020202020002010302030200010100000200010301020102010201030300010000030100030300030203020103020102010002000003010203030003010303020300010100030",
		"00002020300030102020100000103030201020",
		"1000101020303030103000301020002020200030",
		"00200030000000201000100030300010001030100030202020001030001010102010101020303020001030102000302030100000300000302030002020",
		"3000200000301000103020001030102030102020303000300010101020100020201030",
		"100020301000003020200030302020303000300030003020002020202010101030001030001000003020301000",
		"002010101020002000202000003020200020200030102030300030",
		"20202000201000200020001000",
		"303020203030001030302010201020303030202000002000101020002000302030001020001010002010003010300010200010302030201000001000300010203020302020",
		"3000203030302020001010100030200000300030302030302010302010101010201000001010201000103000000010303010100030101030202020",
		"00000020303000",
		"1000000030301010300020003000001000202000203010002030302000",
		"1000301010001000302030002000300010100010100000200000102000002010303020001030100000102030003010103010202030102010200000100000201030100010300010201010203020300020300020103030202030202030203020201020",
		"00000000001030202000200000",
		"200020200020100010101030000030003020003010300010201010003020000030000000200000102000302030202010302020102000301020100030200000000020000010201010003010101020103030002010300010000010",
		"3000300030002020203030201030203000103020302010000010301020301010000030202000102000203010003010000030103010003000203020100030100020301000",
		"301030301010303010001010000000001030102020303020001020003030002010303020103030200030302030003000202010000030003030100020",
		"200030200000301000300010203000001030202020201030300000302020100030101030203020",
		"00101030100020303030201010101030203000002030103010200030300000100020202010101010200030001010302030000030201010103010102010201020202030200010101020202020000000200010",
		"303020201030000000203000303010300030101030300030301000002000003010",
		"3030000000002010",
		"101030203030301010002000100030202030",
		"201030000010102010200020102020303030303010303010000030002020001030003000103000103010000030000010201010201000102010103030",
		"1010102010001010202010102000103020303000000020003000303000",
		"00003020201030302020201020100030000020103010",
		"002030102020200030301030001020",
		"30302020000010002010000000300000101020102010202000001020203020000030203000302020301010100010301000101010102020101030203010001000300010003030001010200010303000200010001020003020",
		"10003030002030301010200010",
		"201010",
		"3030000020201020201030101030001020300030300020000030302000102030003000302010100000302000102010003020202010203000301020200000203030003010101020302030300020001000202030003020103030103030102030201010",
		"1000202000101010002010202010",
		"001010302010303000100000302020202000301020200010101020",
		"202010200010301030003000303010100000103020301010200000001030100030001020203000100020",
		"30303030100030201010003020301010102020003030303030001000202010200030302010",
		"1020300030001030202010002020201010301010100000303010001000000010001010102010100030302030301030000020201000000010103010002020203010200010301020000020200000100020103000001030",
		"30203020001030303000203010301010101000101020300020300020202020300000",
		"0010001000101010203020101020100010001030000000201030100030201030103010000010302020100000202010300030100030003030003000200000201000202010003030102010103010301000",
		"00103010203030302010302020302020200000303020302010001010001010102030302020102020000000203000001030300010102000300020302010102030101010301010302010301020100030302020102010303000203000",
		"200000300000103010202020201010103010300010301030003010102030100030303020100020303020002010",
		"10002010302020001030000010302020101000302030300000100030103000000010001030303020002030300000100010001010202020201030202000001020303020100030",
		"10001030001030300010303000203020002010303020102030102000002000100010302030101010300010003030303030201010202030102030303000000020100000200000001010303010101010",
		"303010202010100000001010303030002010101020103000201030303030201000100000102030202010000020203020003010203010202010300000101000101010301010202030202010201020202030300000103020",
		"000020203010103000201000301000300020301000203030300030101000200010102000100010103020201030200020202020302030301000300000203020001020201020102010302030000010300020",
		"3020",
		"10100010002010201030200020103030102010300020100000301010001010303010003020200030200000101000100030103010002000101030100030000010203020201020102010101020101030101010",
		"30200010000030000010103020100000202010103030300010303010203000103000200010200020002020",
		"00103030201030202030103030203030202010203010302000201000203020101010003000101010301030100000001020201000303030",
		"001030000000200030200010203020201030102030102030302030301030302020300000000000300000203010203020103000202020101020303030300020202030202030003020002020301030",
		"303010301030302030200010003020003000201000003030101000",
		"002020103030203030303000202020202030302030003030001020101010",
		"2010302030101020300020303000101010100010301020302000001000002030",
		"00100010000000301000200020000030201000300010101030203000002030300000302020101030001010201030302030001030202020101030002030200030100000202000000020003010002030001030",
		"202010201020302010301020103010202000102010002010202010001010300000002000102020201030100010",
		"3020100000202000303000103010301000101000101010103030302000200020103030201010000030003000103000302010202010002030303020203010103000200020303020202000002030200020101020203020300020202020201010",
		"0030100030300000301020101010101000002000302010000010002010102000101010001010301030001000300010200000000010102030103020200020200030101020102010102000",
		"30000030001010303010301020002020202000102010203030202010000030302010100030300000302030303030300020002020000010101020100010101020203020303020101010201020000010302020",
		"100000103010301000",
		"0030301030000000103010203020103000103020003020",
		"1030300020100030301000000020302010300000202000202020003020302020303030003030102000",
		"303010101010200030102000303000203030303030001000303010301000002000301010101000301000101030000010303000000000102010002030201020301020101000203030200010",
		"00003000000010102010102030200000001000300020101000102010203030300030103030002030303020000030001020100020101010203000301020003030101020103010301020203000102020300030001020101000101000201030",
		"20202020300020300000202000103020102010103010",
		"002000202000000010000020000030200000003010300020301010102010200030102010101020302000",
		"1010",
		"303000002010103020303030001020303000201030000020300020300030101000000010200020000020",
		"0030000000200010101010102030303000200000102010302010000000103030002010201020000010203020303000100010002010",
		"203000202030002030002020202010000020201020001020002030000020003030202010002020",
		"002030202010103000100030203010101010003020203020002010303020201030302030102010300030203030203020001030300000",
		"201010003000",
		"00301010000030300000001010101030203020302030000030302030101010103010001010100020",
		"301020001000003000103020300000001000201020000030202030002000001030100000000020",
		"303010200020301010100020100030102010201000301020001010101010101020300030100000102030300020100010103000102020100030202020103010200000200020100020000010202020",
		"2000303000103030003020303030301010200010303010301000203010001030103010001030301010003000103030202030202000100010001030001000",
		"101000103010203010103000100010001000302010000020203030102010202010301020000020100000300010302030000030001030203010103030301030001020102020001030203000101030",
		"303010100000001030303000000020203020301010200010302020302010002030000000200000002000",
		"20201000003000203010102020302000203010101010100010202030300000201000202000103020203010302020",
		"200000102030003020200020303010302020203010",
		"3030003030202030001030201000000030201020003030302000200000301020203030",
		"102000001010002020003020002020103030100010002000203000103000000020300020100010303030203020301000300020000030101000301020002020300020300000",
		"102000202020100010202030101010100030303020103010202010000020201000101000200010303030200030202020202010300030201000003020000030003020",
		"3030303020200010303000102000303020203030103010003030303020201000101010102030001030202000002010201010101030302030100020203000001030302000201030002030002010300000",
		"0010001020203020001010302010301010",
		"0000102010203030301030103030000030001010200020102010000010300020202030000000202030000030102000100000303030003010202010302010001000102010100030102010302000001010200020102010202020201000002000300010",
		"10001020",
		"00203030303030301010202030100020001010202030103000201010101010001010201020101030200000302020103020301020300030102030100000201000103000000030001030300030301000003030003010",
		"101010202030101010100000200030003020102010202020102020203010",
		"2030000010101000203010300000202020301000",
		"300030102000100010101020202010101020101030200020101010303030100000203020",
		"203010301030302010",
		"203020101020000030102000100020203020300000201000101000103020302000302020300020003020000020100010201030000020300030203000303030202000102000003020201030302000100010203010200030301020002020002030",
		"303030103020000010302030203010200020102010201010202000202020101030103010000010000010103010002000000020001020200020103010102010201020003000300000200010001030102020202030302030201020",
		"000030100000200020203030200000000010",
		"101010300000",
		"202020000000303030202010102030302010302020000020102010203030003030001020302020301010200000202030202030301000300030002000301000100020000000101000100020003000301020201000103030200030300010103010",
		"2010202000301000103020000020203010300030302000203020200030000030200030102010301020101010202010303030002030103000001010103020101010300010103020001030",
		"102000301000203020300030302010102010202030200000300010102000200000000030200000203020303010101030202000",
		"20301000303030001010300030002020000000102020200010203030100000101030003030200000003000101020203020200030202000103030100000",
		"1010002020201010201000202010303030300030201000302030001010302030103030002000000030203020001030103020300030203030002030101030303020",
		"3010202000",
		"00000030003030100030203030303030201000102010001010003010000020300020001000303000303010202030003000100030",
		"100020202000001010102010101000100010103010100030",
		"300010102020202030100020102010202010101020003030303020201000003030002020201010102010102020300020301010100020100010002020100030200010303010201010000030303000102020001010",
		"001020103010001030100030100010100020301010303010302010300010200030101010001000201020",
		"30201030103000",
		"2000102030101000003010001000202000202030302010000020102020302030201010001030303030300020101000103030201010200010303020202000201010002000003000103030301030003010102030103010102020002020002020",
		"3010202030303030103020103000303020003000103010300010203010103030100030302020200000301010200030202030001030000030202010003020102020102030200030100030001030200030200020102010",
		"2020301020003030303000301020003010103020202010202010303000100020103000000000",
		"301020201020",
		"0020100030100030101020301020100020100020100030",
		"103010101020102030000000303020302010301000200000003020102030201010002000200030301000302030101010001020201000200000000030301020003020101020000000200020103020002030203020100030000000",
		"100000300010002020302030202020001000000010301010203020201030202010202000301000003030",
		"003020302020",
		"10102000301010201000200000302030102010200030201030002000203010002020102030303000301030100030001030000030100010102030001010001010303030200020100020001010303000201030202030303020300030202000202000",
		"3020102020103010300000201000203020202000102020002010",
		"0010201000100030002010201010201000301000301030101020003020103010001030103020202000103020003030003030301030201020102000103010103030200010301020201000301010203030103010200010003020102010301020",
		"100000100020202000201010101020002020303020201010000020001010103000",
		"10101020000000002030301000003010100000202000102000002010002010102010200030202010003000002020002030103000003020001010102020",
		"300030000000301020100000000020000000101020003000100030103000302000000000302000103000200020101000300030102030001020103020301030100010103020302010103030103010003030001010300010301000202020302030",
		"3030202020103010101000202010101020301000300020103020303010003000300000200010103020",
		"1030002020002020102010002020101010300010203000103030002000203000000010",
		"10203000000010002010201020202010002020301010200000303010103030003030102030203020302020000030100010002000303020301020200030002020100000102020001010002010103010300020302010101000202010",
		"30000030101010301020301000002000301000003030103030002010103020301030303030103020002000200010001030200000000010101000103000200000100020201010",
		"10203030001000200000101000001020302000303000300030203020001000203010101030200030100030103030100000301030300010",
		"002020101010300020303010300020000010002000000000200030101020103030302020103010001010100000302030100030001010301000",
		"1020000020201020202000303000002010002030303000100010100020302020301000001030003000303030102030001030103020101000103030201020002020200000200010200000",
		"2020102010101020102030001030101030203020302010200000100020301000303020103020103030000020301000202000100030203010203000200010300030",
		"300000303020203000103010",
		"201010102030303010001030101010102020302030001030102000003010302010202010102000202000302030200030",
		"302030301000202030",
		"302010001030202000302020301000003020201020100010",
		"2010302000003010003000101010103010101020001010000000101000300000202030202020003000202010103010000000003000201010003030003020200010203030202000001030202000003020203030301010200010000030102030201020",
		"3030100000103020203030201010203010000000301030003020200010",
		"10201010203030303030300010002030100010300000103000",
		"30201010100010002000003020302000101020103020301000001020303030300010201020000020100020000010303020002020301010301010301000103020103020",
		"10303010201010302010100000301030300030200000202000101000200010102030201010301000303010003000203010301010200010003030100030003000303010103020",
		"20303030301010",
		"30002030002000100020003030100000202030002030300010202030000010102010100000002020200020300030203000203020301000",
		"30102000003000301030002010001000101020",
		"00103020200030001020201020300000203020000020303020301030202010003000003030",
		"302000102010200010103030303020202000001020200020101000303030002000003020302000103000303030203000100020",
		"002030200000002020101000002030200000301000101030203010301010102030301000000020200030003000303000302000002000100000301030001010200010002030303020003000301000300030203030300000101010201030",
		"1000100000300030202000203000101010303020301020202020203010003010101030002000202000303000203020203000103010203020200010301000301020003000100030201000101000202030103000000030",
		"001020003000202010001030103030203000102010200030300000003010",
		"2000300010102010200020",
		"00301020303020202030302000000000102000201010303020100010203010303020103020002000100000201000002030200000201010201010203010201030301030003000",
		"2030000030100000302030102020202010003030201010002030203010201020100030002020002020301000303010201020",
		"002010001030103030100000203010100020303000300000103020200010301000301020202000102000300010200010303000202020",
		"3030303000101020000020101000000000202000302020201010101020100020303020301000201010002010302030103000000030101000300010301000002000000000302020",
		"20300020",
		"10203000301010103010000010200030002000203020003020000010102020200020201030101010002000303010102010302010003010101010103030202000001020302010203030203020000020102020",
		"0020301000303000001000301000203020200020201030003000100030003030102030202030102000202030202010202030200000200010300000101020201020302020102030302030200030303010202000303010201020003030301000",
		"002000002030200000000000001030103030101000200000003000000010003020202000",
		"3010300000201010001030201010",
		"3020202010203020201030002000302020100020303020201030201010000030301010300000",
		"003010002020303020002000301020002010301010002030302020303020203020002030200000200010200000101020301000202010001010202020203030",
		"0010",
		"002010301000300000301030200010002030201030100000200030001010202000300030302030101020301030",
		"10200020003030300010202020102030301020100010102000002000002020200000002030203000101020203010001030003010000010100010301000302030003030303030302010302020301030003000",
		"1020101010102000103030300030203010002020302030300030000020301020300030103010102000000010102020300010200010103000200030200030202010203030002020002000",
		"1030002000000000101020001020101020203030301020101030002030300020",
		"0030300010303020001010302010302010202030103000303010102030101000101000203030001010203010103010003010001030000030203020103020300030302020203020",
		"30001030200020002000002020202030300020001020101000101010301020300000101030002000003000102010202010200000200020302030002000300010302030003030",
		"203020002030100010202010003010303030100020302030200020103020002020100010201010203010300000201000001020003020203030303000300010202000303030002030002030",
		"10100000203030",
		"3030302030003020301000000030101000001010001000302020200020100020100010102030003030102030302030302010000000203020303020302030100020303030000030303010301000101030303030203020",
		"301030102010102030103020202030001000303020200020202030000020303030202030000030202020002020301030101000103010303030303010103020002010300020101030202010300000003000201020001020100030000020",
		"30200030000030303030200000100030101020000020003010201010303020103020302000300030103030000030102010202020301000200020301020303010000020103000103030200020202000102000100000300030303020003030103000",
		"3010200010102020203000200020203020301000200030001010101010",
		"202020003030101000100020303020300010202010301030200010003030303010101000301000102000303030101030300000103010300020001020302020",
		"3030101020103030103030300030203010101010102010101000201000203010302010201010103000",
		"3010303010201000101020202000103000002030100010",
		"203020303030201000202030100030103010200030100030200010",
		"10301010300030001010103010003000302030003010102030",
		"002000301030201030202020200020000010301000202020100030203020301000103000202010303000300010203010203010103000303010200030001020100000300000202030100030",
		"30003030101000203000102030201010103010300030301000300030102020003000202000300020000010103000002000300020002000100000003030203030303020",
		"300000001010203020001000203020200000100010301020003030202010100020201010",
		"301000201010300010003000203000002000002030102000103000103020000010201000100010002010200000203030200010200020201010100010000010101000101020100000202020002010100030202010302020103030200000",
		"00301000003010100020102000100000001000202010000020000000103010",
		"20202010300020303000202020101010200030301000",
		"00302000101030100020300000103000003010301020102010300020",
		"00103030000030103020100010003000203000303000200000203030100020202030",
		"10200010102020101010202020302030100030003020200010202030303000101030301000201010200020202030303020101010102030103010101030303030001020003000101000202000302020001020",
		"003000001020301030003000302000302020301000201000102000300030002030002000202030203020203000102000300030",
		"30103020002020201030101000102010202010003020100000001030301000002000002000",
		"30301020303000203020202010000000303010101020103030001000203000300020203020103020203020000000203000000010000020300000001030001000303010301010001020",
		"10101010003000001030001020",
		"10002020203030203030101000001000301000000030002000103000001010000000100000302020100000203030003010200030203000203000003010003030000030200000200030",
		"00203000201010303020203010301020201020203020100010102020203000102000301030302000102000103020301020103030301000102000201000000000201030003000102000301000102000300000003010300010",
		"001010202000200000003010100030300000301000002030202020200020101000202000203010302010103020101010201000100000001020000010200020201020100030",
		"301030201030200020300010202010300000103010003000203030300010202030002030102010300010303010000010201020102010302030001030002030301010301000001000102020301030",
		"20100020001010300030100020200010301000303020003020",
		"00201020302030301000000000200000103020003010303000202030002020101030200030200030301030102020101010301000302020303030103020303020200010300030301000103020",
		"0000201030301030100010303000203010100030203030201020202030302020200030100020201010101010103010302010200000200010100000203000001000100010",
		"001030302030102030201020200010300020003010003030102020",
		"30100000001020202030202030203030002010201020102020203000203010203020003030202000202020003020302030001020002020302030001000300030003030",
		"0000202000203000000020",
		"0020103010000020101000001020203030002020303000202010203010200000201010000020301000201010000020203010002000302000001030102020103010301010202030302000",
		"1020101020003030",
		"0000103000002020002000102010000010303020",
		"1000001010300020001000202010000000300020103020000030200000003000303010001000302010302030202030003010200020003000202020003020100000300030303000303010101020303020",
		"102030300000001010103030101030301030300020201000300020302000003030002030200020300020000010202000100030103030102010002020103010003010102000",
		"102020300020201000302030300030302000200030101000203000100000003020",
		"10002010301010002010003020300020301030201020200000300010002000303030",
		"101030103030101010303030300020203030201030100000102000302010100020003010002000201000200010100020201030300000301010202000103010100000000030302000103010203020301020102010301010103020002000000000001030",
		"0020202000103020102020203010302000002020001010103010101000203000003010002020103030302020101010",
		"0000100010202010203020101000101010102020302030101030200000300020103030100020203020003030003000201010201030000000003030000020101010303030301020",
		"10001030003010102030100020100010202000201010003020101020003010200010002000000000203020000010003000202030301000102000300010300000302030302010003030101000100000",
		"3010002020202020302030301020203030203020100010003000000010203030100030002030202030101020301000302020300000300010102000",
		"103020100030303000203020102020201020201020200000103010100010301030201000202030202010101010102020201030001010102030300010202010200000301010100020101010303020002020",
		"0000101030202010300020002030101000303030101020203000201000201010002010002010203010200010101000101020201030",
		"0000201010100030302000101030102030200030003020",
		"300020301030101020303010103010001010001010201020",
		"20000020302010101020100010002020201010303020201020303000101000103030003010100010102000303000",
		"302010003010100000301010000020001010200010102030100010002000001010303020101020202020302000203020203000201030202000200020102010",
		"3010203020202000001010301000103010202000002000102030301000302000300020000020103000202020102000200020302000301010302010203000",
		"1000300000",
		"302020002030201000303010301020202000103010",
		"3030002010203000303020201020003030102020302010002030303020202030302010102020101020003000100020302020303010103010202020302010200030101000202020000020101000102030202000301010300000202020002010",
		"1010100010301010103010001010102030201010302030002010000010",
		"2000000010102010103010200020103030002030202000000020002000003010200000001020101010201030003020100030200030103020101000",
		"20301000203020301000201020301010102030200000",
		"00300010200020002020003010001000",
		"00202030002030201000102020202010003010303000301030002020003010303010001030201010200030103020202000003020300020203030102000102030",
		"000010",
		"0020302030100030303030000030100020301000000000202010301000202030303020300030102030200000202010201000203000302000301030",
		"103000000020302000201000201010303030201020101000203030003030302020302020301020202010000020302030203010000010302030003030302030100000201030",
		"30103000300000103020100000000000203020002000202030302000300000103010300000",
		"30100010000010",
		"102030100010001010303010102010100020003010001030002010000030203000003010002010000020302000300020003000302000",
		"20200020201030102010100030300010202020000000100000302010101010301030000010202000103010002010100030003000100020203020101000302010203010103000301020201020001030203010200020",
		"200030000030203030202030303010203030300030103000200010202010303000303030103010201010",
		"3030003010102010202030001010003020301000100000103020002000200030200020000010200030303030100030302020201000201030102000100010200010203020000030301000",
		"0030302020202030103030002010202010303030303020103010001030000010200030300010300010302030102010200000203010103030002000001010003030002000000020001030",
		"002010300000001030202010101000100030003030102030000020201010003020301000102020001020301030202010",
		"10001010102030302000",
		"201020000020301030203000201000101030103000101020202020000030101030000010001030103000300020200000303000303010103000203010103020100000303010000000103010102030003010300020",
		"302030003000301020202010302000301030302000101000203000303010301010002030302010003020103020203020203010003000202030202010",
		"20203030001030000030200020000000201020100020100020002000001000303000303010302020001030301010103030003000003030302010200020202020100000000000203030000010",
		"00301000301000103000001030102030000010002010101010102000303010203000302030100030203020301030300030300000300030301020203030101000002010102010203010002030003030100010303010002030001000202030301030",
		"30003020000030200010303020102000",
		"003010200020200020203000202010100020103000003000202020",
		"101030001020302030000010203020003030202000000030000020202010000000201030200020000010101000301030302000301000100020003010001010303000001010002020202010200000301020102000202030202030202010",
		"2010203030303030301010202010302030102020001000301000002000100020102000302010203020002010102010303000000020203020103030003020203030201010102020201010103020101010203030000020",
		"30203030100010300000002000000030001020101010200030003010102030202000001010301020101010301030100030301030203000102010302030003020003020101010003030001030302020",
		"1010",
		"30202000101010203020203030",
		"3000200000000010300030201030302010003020002000000030003010200030003010202020203000202020103020001000201030201030203000103020200020",
		"100010100020103010000000101030202030100000303020000010100030100030103030000010200030000010102000001010302030300030100000300010202010302020000000",
		"3030001020203030202010303030002030203030003000301000303030101030103000202020300020200020001000002010203020100020300020203020101000203030000000202000100010302020303010002010002020203010302030",
		"002030101030102010003010203030001020",
		"101010103030202010001010202020200000300030302000000030301000101030203030300000200010001020101010301000302000202010201010101000301010101030000000202010001010100030003030",
		"10002010003030202030100030101020002000303010200020103030202000101000000030003010202030003020102020302020100030301010200020201010003000102030201020100000100020300010203030302010101000100000002030302010",
		"2000000000201010300020201000",
		"10000020301020301020301010101030100000203030302000302030",
		"0030200000100020100020001030103020200000100000002000001000201030000030300010002020201010200030300010301010202010100020102020102030303020300010302000001000103030102010100030300010101030",
		"302000300010",
		"001010202010003020000030301020203030301000100030101010202020202010200020302010302030201000101030002000",
		"301000102000100030201020101030303030202030302030303030000000001030101020103030001010303000302020000000301000000000303020000030",
		"201020100030300020100030100000200010302000200010201020002030203020100020001000301020301020302030101020",
		"00000010301030200010002020002020203020202020002020203020",
		"201030100030002030301000203020203010001010300010301000203000202020101030003020302020000020000010200000001000102030302000300030302020202010000020303030200010301010003010100010100010001020001000203000",
		"30001000202030001020301000100030201010303010301030001010101010",
		"1000203030103010102010102020301000300020303020100030202010001020",
		"301000101030100020301010300010303030302000203030201030003030101020000020103000202000103020002030100030200010001000101030203020200030100030002020101020100020003000100000201010202020",
		"20201030003000100030103020300020200030301010200000302010203000300030102030301020202030300020002000002000303010102020203000000000300000202010202010001020103010",
		"100030102030001010003010200010201020202020101030300030000010003020102030100020203000002020103030300010003020003020301020301000003030002020300020",
		"302020100030101030303010102010300000200020100000100030300010003010000030300020202030300010001020003020102000101000200010201000201020203020200000202000003010303020200000100010103030300020102020000030",
		"002020103030200020103010302020100010201030202000303020101030001020300020002010103010102030202020",
		"30301030303000202000101020302000101030203010302030100010202010103000100030102020300030302010201000300000302020201030001030100030",
		"101020103000000020300020200010101010",
		"1010100030301000303010003020000010303030003010302030201030301020100020300020102010301010",
		"3000203000300030100020202020103020102000103000002020302030302000003020002000302010102030001000000010301020102030002010002010202010201030101030000000100020303020",
		"10000010002020103030200010102030002010103010100010301030001010001010200000000030103010200000300030301020200020300030202030302030003020100030100020103020203020101000301020102000102000",
		"2010",
		"10302000301030203010303010301010303020300030301020300030303020303020101010002010001020203020302000101020000030003030001000303000201010001020101020301020101010102030003020",
		"1020201020000010302010002010202030303030102010202000301000200020003030103030203020103020",
		"000030201020101020102020101020000000201030203030301030202020100020300020",
		"3000303030102030303010000030303030201030203010301030202030200030201030",
		"1020203010002020200030202030203020103030001010100010003030102010203010003010003000300010302010301020103030",
		"002030101020301000201020",
		"201020302000203020",
		"3030003010301020000010202030000000000000002010302020301020300000200010100000001020102020300020301030302010002010301030303010003020302010100010003010200010203000300010",
		"102000303020002000100000100000101010203000300020",
		"103000300000303030001030201020302030001010203000002020101000200020",
		"10103030200030103020003030201010103030001020002010100030201000201010002000103030202020001020103010201030",
		"2030001020300000200000303030201030302000100030003030",
		"3000002010",
		"103000203030301020202020102020102030200030001010200020303010000030300020300030001030000020301020101010303000",
		"303030000000102010101030200000",
		"00203000301030301000003020",
		"200020202000303000300020003010102010000020200030001030101020100020002020",
		"20001010303030002010303000200010202030201000303010",
		"10100030",
		"2020302010200030202000202000002030003020301000002000103020201030300030102000103000203010302030101000100010303010202030101010000030001030301000201020201020003010202010000010001000",
		"10003000302020202000001010102020101000300030202030302020001030003000301030201020302020002000",
		"2000200030203010003000102030000030000010300030301030000030202010202030301000000000100000001030202020",
		"2020203010101000300000003020001030303030203000200020201010103020002010301030003010",
		"1010",
		"20203020303000303010300030103010100030203000101000100000001020202000",
		"003000100020002000300010300000201010101010200030",
		"3000203030202030102010002010000030102020301010300020203030102030001020203030201030002030100010101010302030100030003010102030303010102020300030103000102020000020200030002010001010",
		"20202000203010101030001000000000100000202000201030302010102000200030102000000020302000202010201030101020201020202030101020103020303020201010203010201030100010202020000010300010101010",
		"003000300020103000203030303020000020",
		"1030101000202020300000000020201010301020301030301000102000301020001010002020002030200000301000000030201000201020201010300030300010103010101030203000200020000030301000200000",
		"0020203000000000",
		"10301000202020102010100010003000000000101020201010201010001030303020201010102020302010000010002010100020002000103010102030300010203020102000203030202000202030103000301000000010",
		"0020303010303010102020000020302020100010301010200010303010303030301000200010202010101010003020301010003020101010103020301030101010001010001030302000303020000000",
		"3000100030002000000010303010203010200000300000101020302010103030101020003030301000",
		"10003010100000002010003030202000",
		"10201010102010000020301030001030303030001000101020303030003030003000",
		"2030301010003000003030002010001010201020300000303030201020302030200020202010202000101020001020201020300010200010000010101010000010102020303000200000302030003020002030203010100020",
		"303020303000201030300010",
		"2020000000002010102010003010002000100010201030200020102030100030300020000030203030303020102000001000102000201010201010100010303020302030203010200020201030301010300010200000201000",
		"003010103020102030202020201000000020000020002020",
		"1020101010003020102010303020302000002020003010101020100020200000003030301000301010301000300000202000000030101020000020201000201020103000203010202000102010303000302000103020003010203010302020",
		"001010303010200010100000102020003020200030301030300030000010202000300010002020101020002000301010201010",
		"302020203020300020200020000010101020101020303020101020301010203000101020300030003030201000201000301020100000201000000000300010201020300010303010302010102030202010303010001000",
		"10002010000020002030303010103030001000200010102000002000103030",
		"1020101010301000002030102030201000201010202010003000003030001000300010101030203020101020002010003000202020000010203000103010302000301000002000100030203030000030303000002010001010100030",
		"301010203030002030001020002000003030302020300010002030103020002000300030101010002020301000100020102000303030100010200010303010202010003030000020302000103030001000",
		"103010300020000000103030203000200000300010202000000010303010303020100010003010302020102020102030100030103020303020002020203010100010003020000000203030001010",
		"0020301030301020003000200000200030303020202030100010000000002010100030001000303030200010",
		"2030",
		"10200000203020000030203000300030203020300030102000001030100010202020303020201010303000202020301020203000000020303010201030100010",
		"20000000203010002020002030003030103020102030303000001000200000103000102000002020300010201020103020101030102000201010101020",
		"00201020301030002020303010102020003020101010301000201030303020103030302030100000300010200020302020200030200010",
		"1000001010101020203020100010303030003020202030000020200000002010103010001020003010001020103030100020202030102000301000300010302020101030101030201000101000",
		"0030001000200030201010003010003030302000002000301030000010300000003020301020303010200030300010200020001010202000200000002030100000001020003010303020",
		"2030200010201000000020300030303000301030103030303020003010102010100030301020301010202030200000303010102020302030102010302020003030101030200010100030202000001030200000201000001020103030203000",
		"103020100030203000203000000000301030103000302010003010200000001030102000200020",
		"30",
		"00103000002000201010000030300030303020002000100030302030102030002010103020203030200010203000202020",
		"20001030300030002020002000303000201010202000200020102020203000001030202000001020300020000010103010301020001010201030102000300030302000102000201010303010201000003010203020",
		"2000002030003010301000103000302010103000003010303010002010300010100000200010002030203030303000302020302000302010",
		"000030103020002000100010303010302010001030002010003010302010001030300030300000102020301000003000102020002010103030003020302030000010301000100030101000",
		"30300000302020001010003030202000100000001010301000",
		"100000003030303030100010302010100020101020102010301010002030003020201000301020101030103020100010102030000020103010100020101010000030301010203010101000302010001020102010302020",
		"2020203030102010302020300020203000302020103030100030302010100000",
		"30302010302020200020201020303010100000302030002020000030301000201030102010302010003010001020101000001010203030100030",
		"3010002030202020301000201020101000003000002000103000302000102020102010101010002030201020202000",
		"1020201010203030002030",
		"0030302010301020303010103010",
		"20301000000030300000100000103030001000103020302010302020203010301010202020103020103010100010003020203000200020",
		"10101010302020001030100020303000303010103010303010301000000000200020003020000010002030002030203000302000101030303020",
		"10002020200020001000300020302030003010000020003020100010301000303000",
		"3030200000101010103020201020",
		"302000201000301020303020003020202030",
		"20100010102000000030200000003010000010303000300000002020000030000000100030303020300010003020301030203030",
		"2010001000303030002000302020101030003010102010302030301020",
		"100030300020103010201000201020302010303010300020200030202000000020200030103030001030002020001000200030202010300010202000001020202030",
		"101000101010",
		"1020301030201020203000302000101020200000101020001030201000",
		"3020303030003000",
		"20102010002020201000202010001000300030301030001010202010",
		"2030002030103010303000103030203010002000301000103020101000101030302000001020102000203020002000000000301020300010300010202030303030",
		"3010200030301000000030100020201020000030101000303030301000101000101030001000000010303030202030101030200020102010201010102010",
		"00303000103010302030301000300020203000303030000020200000102030",
		"003010003010102020203000101030003000",
		"0020000020002000101000203000201000",
		"301020103000101010100010002010001030101000100030103000101000303010102020002030100000303030001020302010",
		"302000103020302020202010301030103010303000101010100000200000202000002030003010303030001020101030201010103000103020303030103030101030103020201010302020100000303030",
		"0030300030202010201030100030303020303030003010100030203020101020003020300010102000100030200020000030",
		"101010302010301020101000102030303000000010202020101030201000103010100000201030301010000000202020102000100030003030000000002030",
		"300030001020102000201000300010301030",
		"1010000010",
		"3030",
		"30100020301020302010101030301020002030103030101010101020200010203010101010101010301010301000302010300020201010000010300030101020000010200030102020303020302030203010002030102010101030",
		"002030001030001010001020002020101010103020000030300030202000300010003030001020",
		"0030201010003000300020102020302000201020103020202030203030000020101020000010103000001000300020",
		"2020300030101000",
		"20003030001030002030103030002020003010",
		"103020301010200030202030103010002010200010",
		"10003000103010202020200000102000300000000030101000102010003020201000",
		"200000300010102010301020203020103020302010000030003030200030303010301010100010300010300020203020001020000010302020101010",
		"10103020",
		"1030301020103000301010000020203020200030302000000020102010203020203000203010202010101020302020101000303000001020",
		"2000101000201020203010203030200030102000102000003010302030102010100000302030102030101010200030200010101000002030301000000010302020302000000000",
		"00102010300030103020000020001020303030303000301010303030103000201030001010001020103010200000301030302000302010203030302020000010300030",
		"103010001000103020202030102030001000101000002030201030002020301020103010",
		"201030301010103030002000003010300000002000303010300000103030302000002000200030102030101000300010201030100030200030200020301030202010300000301000003000300010001030100000203010101010100030",
		"100010203000302030303030003030201000",
		"10103010300010102030003030102000203030001000002010100010001020303010101030203030301010200030103010002010002000200030302000302010003030203000",
		"001000000020200020301000003030101000200030302000001000001030203030100000103030102000",
		"201020003020",
		"102030303020300000101000002000101020",
		"102000200030003010002020302020",
		"202010203030102000202000303020202030203030001020101000",
		"20303010203000201000101000302020201030100020300010002020001000002010203010100020",
		"0000101000302010101030102020102030200030100020003020",
		"2020002010003030102000102010201000000020300020303010103020300010000020200020201020",
		"2010002030300010000020101030201020201030001010002030202000102030101030003030302030201000001020103030200010000020200030202020300000203020003000",
		"000020203000200020303020002010103020300030100000302000102020300000303030100020301010201000000030300020",
		"0010000030002020201000302020202020101020001020002010102020200020102010000000103000203000001010",
		"00001010000020101020",
		"30000010001010200010302000",
		"00103030103020000010000000002020102020303020200010303030003030300000001010302010201020200030000000102020201020302000301000102020",
		"102020103010101010",
		"30103030003020103010302020001020001010302030103020102020001010301010",
		"20",
		"10201030101000000030100010100030102010302000303020201000302030102030301010101020100000000030102030101000301020201020200000100020203030201010003030102000001000",
		"0030303010100000102000303010200020001010200030003020102030100000003020002010102020301000002010000000003030100010000030301020000000000010103010",
		"10303010301010103010203000200010",
		"101020000010103000303030000000100030301000100000101020202030002030301020203000102000102010100020000010100030101030200000101000300010002020200020",
		"3000100010001010103030201030103010002030202010202000103010203000003000300000101010300000003010101020000010101000301010",
		"2020300020000000203020201030300000000010000030003020202010201000303030203030001030101030203000000030203000302030000000301030201010102020101000002030100010000020103030302020",
		"100010001020202020301000001020003000102010101020100010001020301030301000303000",
		"0030301000301010101030002000002010301000200020103000203010300000003020203020303020103000300020002020200000300020201010200030003000",
		"3020300030002000001000301010003000200000102010000030203000001030201010302020101000001020100030100000300000302020301000301030",
		"20100020000000302000003000202030302020300020103020200020303010001030200030002000103010003020200020102020302020302000101020101000202030103020100030103010100000202020203000100010001030301000302030",
		"000020300030300030300020001020101010202030301020201010300010101000102030002030100000101030200010",
		"20202010001030300030303010001030300010303010302020",
		"0000302020201000100000003010302030002000202000002030003000203010303010101030200020303020300010100030000010303020100030300020102000101010200010202030102020100030",
		"103020101000103000201000103030101020200030201000202010103030100010300030103020001030300000101020303000",
		"30102010202030302020303000003030100020303000303010301000002020101020100030",
		"00103010102000203000",
		"30301000003020301020203020",
		"0030102020000000300000202030103030201010102030100030302020003010100030102010102000003000201020103000002000000020200010301030",
		"2030002010102020001000201030103030101010001030101000203030101030002000300030001000102010201030202000201000102010001010103020103020",
		"303000302000301010003030301020101020303010001010203020000000102010002020001000001030301010202000",
		"202000003010200020201010302020300010303020103030300010200000002030000000000010300010000020",
		"00002030302010001020200000202020100030302000002010002010301020000000102020100020302020001030201000101010301030203020103020103030100030003000203010001000203030",
		"1010100020102020201030203030202030303020200030302030100030101020001030100020",
		"000020201030301020301010300000302030300030102000303000303000000010100010301020203010201020101020",
		"301000203010002010201000301030102030001020303010003030200020003030000020302010100030102010302030300020001020203010303010203010203000000020103010",
		"00302010100010302010100000203030301010200020002020003030302020300030003000001030100030300020303010102010201010000000203030303020001000103000002010300030200000100000203000",
		"002010000010100020002010200020302010302020002030200010000030201020002000",
		"30203010200000103020103000201030101030000020203010101030101000001010201030200020002020202030100030200000302030002000103020100020002030201020301030202000300000",
		"1020200020101030203030202020100000200000002030201020102000303010000010100000200010301000201010100000302010",
		"201020303010100030100000203020200020103020002010102010003010000030101000100010",
		"2020002030203030103010301030102020002030301000",
		"10302010102010",
		"2010003010101030001030000010301020000000102020302000303000201010101020201020003010103010202020103010203000001000101020103010002010103030103010200030",
		"2030103020002010000030200020103030003020100000203030100010102000303000101000201000102000301030302010000020003000101030003000000010201020101020",
		"203020001020201010000020300030001010302000103030300010202020202030303020301030200030203020001000002030100000000020200020003020302030101000202020001030002000",
		"001030102000103000002020200000101030003020102030000020300000003010002030103000303030102010101010302020300010301010",
		"1020003000103020201010202020003000103000200020002030000020",
		"0010000030300030101030102030102000201030200030302000200010200000201020002000001000001010200030002020200030100020202000200000300030200000103010303000003020102000203010001030200020303010101010",
		"2000002030301000303000101000002020300030203030300020200030100010302030",
		"10001010000010000030301000002000",
		"0000101030300020002030300010302030203010300030202010300010200020202030002010200030202020202010303020002010300010301020303020302020301020000030202000303010302030003000",
		"103010100030301010101010301030200030",
		"303030203000203030303010303010300000302010303010001030002030002010001030203010203030001030003020300010203020001020100010200030",
		"00001010303020302000102020303020003010101000003000202030003020301000302020303000103020203000003030103010003030001000003010002010002000000030300030203010000010",
		"0020303030",
		"200020302000000030000010200010001020100000101020002010301030003030203000001020201000102020200010",
		"302020101030200010103010103030300030202010203010001030001020200030302020002010101020200000103030003020001020101010203010",
		"20300010303030301010302010100010200010302030201020203030002030201010303030200020101020301030301010103010101010103020102000100030203010303010101010202010200030103020302030003010103010201030202020202020",
		"2030302020300020102000300030101010100030301020",
		"003020101020001010001000001000301020102020200020300030201030001010101020002010300010002000",
		"1010001030300030100020203010202020301020102030001010103020003010301030203030200010003010101010201030001030003010302000003000002010301010102010000000103000",
		"203000002020301030200030302010303010002020201030003010000020302020101030001010003010000000002000100000202010303000102030101010003000200010200020000030003020",
		"0010101000202020100020102030303020300000202010103010100000203010203020200010103020",
		"2030102020101020103010202010202020103030101000001010200020201000200030201030003030",
		"2010102000002000101030202030100010201030",
		"100030000000000020300010302000001010200010103010",
		"200010302020302010201030302000303010101020103000303020103030100000103030202020302030200020301030003030203030002000",
		"30001030102030203030301000101030202010003010302030202000",
		"100010003000302020301000202010003000202010101010102020302000202000303010002030300000003000003030101000101000103000303020100010300010301020",
		"20101020003020002000002000003030202010100020301030303020201010301000001010103010000030200030203020200030300010102010302030002010103010103000301030202000201020202030203020",
		"203000200010003000000020103010102030103020203020002020301000200020203010100030103020203030100000103000302030200030102010002020202000003020101030001010300000100000000010203010003010103000100020",
		"002030302020102030301020102010100030202020201010100010301020303020302010102020102020203030301020203010300030100010001000201020002010003000102020100010201020102030",
		"30001020000000200030001030100010001020203030001000001010001010",
		"302020003010001000100020101010302030203020303030300020100000100020301000200000001010000010003030002020202030201030203020102030",
		"00103000000000203000102020303010301010301010102000302000000000100000102010200000100000201010100020103000300000301020203010002000202010303010102030303020",
		"3020102010000010003000303030203000201030101030301010",
		"201000203030000020100020302030101020103030001000301030200020102020001000303010100010303000",
		"2010000010000020201010001030001020302020003000200000102010300020101000001020300000302000203030102010",
		"3000202010000000303020103010200030200030201000200010302020202000",
		"102020303020101030301020002000300000102000200010001010103020202020300000003020202000003000102020000010200020002020103010003000101010001030000000103030003010",
		"000010201030003030301010200000303010300010000020302010103030002030203000000030101030302010100030",
		"20203000302020103020003030000010002010203010102020103010200030",
		"10303020001000003010101030300020002020300000000000103020300000100000000030201000100020003010",
		"0010202030001010302030300000001000001000100010103030103000300000102030303010001020201030001030301010000020302030300010002030301000",
		"101020000000",
		"1020201020301000003030101000",
	};
	//char* s[10] = {
	//	"0001110110",
	//	"0000",
	//	"0111",
	//	"010110",
	//	"1100",
	//	"1110",
	//	"11",
	//	"00",
	//	"00",
	//	"111100",
	//};
#endif

	FILE* fp = fopen("D:\\radixtree.txt", "w+");

	sp_init(mem_chunk, 1024*1024, &tmp_sp);

	rm_init_root(&root, 2, "00");
	rm_init_pool(&pool, (M_sintptr)offset_of(rm_ut_arg_t, rt_stub), 100);

	c1 = clock();
	srand(time(NULL));
	extra_arg.dummy_node = extra_arg.extra_node = NULL;

	for(i = 0; i<LOOP_COUNT; i++)
	//for(i = 0; i<1; i++)
	{
		printf("%d ", i);
		//构造字符串
#if(FIX == 0)
		fprintf(fp, "start...\n");
#endif
		memory_index = 0;
		//dup_nodes = NULL;
		for(j = 0; j<STRING_NUM; j++)
		{
#if (FIX == 0)
			str_len = rand() % STRING_LEN + 1;
			for(k=0; k<str_len; k++)
				memcpy(str[j]+2*k, x[rand() % char_num], 2);
			str[j][2*k] = 0;
			fprintf(fp, "%s\n", str[j]);
			fflush(fp);
#else
			sprintf(str[j], "%s", s[j]);
#endif
			nodes[j] = (rm_ut_arg_t*)rm_alloc(sizeof(rm_ut_arg_t), &pool);
			reg_mem_alloc(&nodes[j]->rt_stub);
			nodes[j]->key = (M_sint8*)&(str[j]);
			rm_init_node(&root, &nodes[j]->rt_stub, nodes[j]->key, strlen(nodes[j]->key)/2);
			
			if(extra_arg.extra_node)
				reg_mem_free(extra_arg.extra_node);
			if(!extra_arg.dummy_node)
				extra_alloc = 1;
			rm_process_arg(&pool, &extra_arg);
			if(extra_alloc)
				reg_mem_alloc(extra_arg.dummy_node);
			extra_alloc = 0;
			
			dup_tmp = rmt_insert_node((M_rmt_root*)&root, (M_rt_stub*)&nodes[j]->rt_stub, &extra_arg);
			if(dup_tmp)
			{
				nodes[j]->rt_stub.parent = dup_nodes;
				dup_nodes = &nodes[j]->rt_stub;
				str[j][0] = 0;
			}

			for(l=0; l<memory_index; l++)
			{
				if(memory_address[l] && memory_address[l] != extra_arg.dummy_node)
				{
					//printf("%d: %s(%d)\n", l, ((M_rt_stub*)memory_address[l])->skey, ((M_rt_stub*)memory_address[l])->skey_len);
					UT_ASSERT(((M_rt_stub*)memory_address[l])->skey 
						|| (!((M_rt_stub*)memory_address[l])->skey && !((M_rt_stub*)memory_address[l])->parent));
				}
			}
		}

#if 0
		for(k=0; k<STRING_NUM; k++)
		{
			printf("%s\n", str[k]);
			if(str[k][0])
			{
				search_result = &(nodes[k]->rt_stub);
				printf("node %s(%d)", search_result->skey, search_result->skey_len);
				search_result = search_result->parent;
				while(search_result)
				{
					printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
				}
				printf("\n");
			}
		}
		printf("\n");
#endif

		rm_matafy(&root, rm_alloc, &pool, &tmp_sp);

		if(extra_arg.extra_node)
			reg_mem_free(extra_arg.extra_node);
		if(!extra_arg.dummy_node)
			extra_alloc = 1;
		rm_process_arg(&pool, &extra_arg);
		if(extra_alloc)
			reg_mem_alloc(extra_arg.dummy_node);

		extra_alloc = 0;
#if 0
		for(k=0; k<STRING_NUM; k++)
		{
			printf("%s\n", str[k]);
			if(str[k][0])
			{
				search_result = &(nodes[k]->rt_stub);
				printf("node %s(%d)", search_result->skey, search_result->skey_len);
				search_result = search_result->parent;
				while(search_result)
				{
					printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
				}
				printf("\n");
			}
		}
		printf("\n");
#endif

		for(k=0; k<STRING_NUM; k++)
		{
			if(str[k][0])
			{
				search_result = rmt_search(&root, str[k], strlen(str[k])/2, RT_MODE_EXACT, &matched_len);
				UT_ASSERT(search_result);
			}
		}
		
		j = rand() % STRING_NUM;
		//j = STRING_NUM;
		//j = 2;
		fprintf(fp, "random j is %d\n", j);
		for(k=0; k<j; k++)
		{
			for(l=0; l<memory_index; l++)
			{
				if(memory_address[l] && memory_address[l] != extra_arg.dummy_node)
				{
					//printf("%d: %s(%d)\n", l, ((M_rt_stub*)memory_address[l])->skey, ((M_rt_stub*)memory_address[l])->skey_len);
					UT_ASSERT(((M_rt_stub*)memory_address[l])->skey 
						|| (!((M_rt_stub*)memory_address[l])->skey && !((M_rt_stub*)memory_address[l])->parent));
				}
			}

			//matafy之后就不能再remove了，因为matafy之后数据结构不符合radix tree要求
//			if(str[k][0])
//			{
//				search_result = rmt_remove_node(&root, str[k], strlen(str[k])/2, &extra_arg);
//				UT_ASSERT(search_result);
//				
//				//printf("%s(%d) is freeing\n", search_result->skey, search_result->skey_len);
//				reg_mem_free(search_result);
//				rm_free(search_result, &pool);
//				
//				if(extra_arg.extra_node)
//					reg_mem_free(extra_arg.extra_node);
//				if(!extra_arg.dummy_node)
//					extra_alloc = 1;
//				rm_process_arg(&pool, &extra_arg);
//				if(extra_alloc)
//					reg_mem_alloc(extra_arg.dummy_node);
//				extra_alloc = 0;
//
//#if 0
//				for(l=k+1; l<STRING_NUM; l++)
//				{
//					if(str[l][0])
//					{
//						search_result = &(nodes[l]->rt_stub);
//						printf("node %s(%d)", search_result->skey, search_result->skey_len);
//						search_result = search_result->parent;
//						while(search_result)
//						{
//							printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
//							search_result = search_result->parent;
//						}
//						printf("\n");
//					}
//				}
//				printf("\n");
//#endif
//			}
		}

		//rt_free_all(&root, rt_stub_free, NULL);
		rmt_free_all_p(&root, &pool);

		if(extra_arg.dummy_node)
			reg_mem_free(extra_arg.dummy_node);
		rm_free(extra_arg.dummy_node, &pool);
		extra_arg.dummy_node = NULL;

		while(dup_nodes)
		{
			dup_tmp = dup_nodes;
			dup_nodes = dup_nodes->parent;
			reg_mem_free(dup_tmp);
			rm_free(dup_tmp, &pool);
		}

		//UT_ASSERT(memory_blocks == 0);
		//printf("\n");

	}

	c2 = clock();
	printf("\nneed %d ms\n", c2 - c1);
	
	fclose(fp);
	rm_destroy_pool(&pool);
	free(str);
	free(nodes);
	free(mem_chunk);

	memory_blocks = 0;
	
	UT_CONCLUDE(ret);
	return ret;
}


#ifdef STRING_NUM
#undef STRING_NUM
#endif
#define STRING_NUM 64
//#define STRING_NUM 1

#ifdef STRING_LEN
#undef STRING_LEN
#endif
#define STRING_LEN 10

#ifdef FIX
#undef FIX
#endif

#define FIX 1

static void* get_rule(void* rule)
{
	return rule;
}

M_sint32	UT_radix_mata()
{
	M_sint32 ret = 1;
	M_sint32 i, j, k, l;
	M_sint32 str_len;
	M_sint8  base_ch = '0';
	M_sint8	 (*str)[2*STRING_LEN+1] = malloc(sizeof(M_sint8)*STRING_NUM*(2*STRING_LEN+1));
	rm_ut_arg_t**	nodes = malloc(sizeof(rm_ut_arg_t*)*STRING_NUM);
	M_rm_root root;
	M_rt_pool pool;
	M_rt_stub* search_result = NULL;
	M_rt_stub* dup_nodes = NULL;
	M_rm_stub* dup_tmp;
	M_sint32 matched_len = 0;
	M_rt_arg extra_arg;
	M_sint32 extra_alloc = 0;
	M_sint8* x[] = {"00", "10", "20", "30", "40"};
	M_sint32 char_num = sizeof(x)/sizeof(char*);
	M_stackpool	tmp_sp;
	M_sint8*	mem_chunk = malloc(1024*1024);
	clock_t c1, c2;

	M_slist	match_res, match_total_res;

#if (FIX == 1)
	char* s[] = {
		"00",
		"001000",
		"0010002000",
		"0010002000300040",
		"001000200030004000",
		"00100020003040",
		"0010002000304000",
		"001000203000",
		"00100020300040",
		"0010002030004000",
		"001000203040",
		"00100020304000",
		"00102000",
		"001020003000",
		"00102000300040",
		"0010200030004000",
		"001020003040",
		"00102000304000",
		"0010203000",
		"001020300040",
		"00102030004000",
		"0010203040",
		"001020304000",
		"002000300040",
		"00200030004000",
		"0020003040",
		"002000304000",
		"0020300040",
		"002030004000",
		"00203040",
		"0020304000",
		"00300040",
		"0030004000",
		"003040",
		"00304000",
		"0040",
		"004000",
		"1000",
		"10002000",
		"100020003000",
		"10002000304000",
		"1000203000",
		"10002030004000",
		"100020304000",
		"102000",
		"1020003000",
		"10200030004000",
		"102000304000",
		"10203000",
		"102030004000",
		"1020304000",
		"30203010203020",
		"3030203010300010",
		"30102030203020302010",
		"3020200010103000",
		"10300020",
		"00302020102030",
		"00102020",
		"0010200020",
		"001000200020",
		"1000200020",
		"20301000",
		"10002010",
		"100020",
	};
	//char* s[10] = {
	//	"0001110110",
	//	"0000",
	//	"0111",
	//	"010110",
	//	"1100",
	//	"1110",
	//	"11",
	//	"00",
	//	"00",
	//	"111100",
	//};
#endif

	FILE* fp = fopen("D:\\radixtree.txt", "w+");

	M_rm_handle* handle;
	//M_stackpool	handle_pool;
	M_sint8*	handle_memory = malloc(1024*1024);

	handle = rm_init_handle(handle_memory, 1024*1024, NULL);

	sp_init(mem_chunk, 1024*1024, &tmp_sp);

	rm_init_root(&root, 2, "00");
	rm_init_pool(&pool, (M_sintptr)offset_of(rm_ut_arg_t, rt_stub), 100);

	c1 = clock();
	srand(time(NULL));
	extra_arg.dummy_node = extra_arg.extra_node = NULL;

	for(i = 0; i<1; i++)
	//for(i = 0; i<1; i++)
	{
		printf("%d ", i);
		//构造字符串
#if(FIX == 0)
		fprintf(fp, "start...\n");
#endif
		memory_index = 0;
		//dup_nodes = NULL;
		for(j = 0; j<STRING_NUM; j++)
		{
#if (FIX == 0)
			str_len = rand() % STRING_LEN + 1;
			for(k=0; k<str_len; k++)
				memcpy(str[j]+2*k, x[rand() % char_num], 2);
			str[j][2*k] = 0;
			fprintf(fp, "%s\n", str[j]);
			fflush(fp);
#else
			sprintf(str[j], "%s", s[j]);
#endif
			nodes[j] = rm_alloc(sizeof(rm_ut_arg_t), &pool);
			reg_mem_alloc(&nodes[j]->rt_stub);
			nodes[j]->key = (M_sint8*)&(str[j]);
			rm_init_node(&root, &nodes[j]->rt_stub, nodes[j]->key, strlen(nodes[j]->key)/2);
			
			if(extra_arg.extra_node)
				reg_mem_free(extra_arg.extra_node);
			if(!extra_arg.dummy_node)
				extra_alloc = 1;
			rm_process_arg(&pool, &extra_arg);
			if(extra_alloc)
				reg_mem_alloc(extra_arg.dummy_node);
			extra_alloc = 0;
			
			dup_tmp = rm_insert_node(&root, &nodes[j]->rt_stub, &extra_arg, s[j]);
			if(dup_tmp)
			{
				dup_tmp->rule = s[j];
				nodes[j]->rt_stub.parent = dup_nodes;
				dup_nodes = &nodes[j]->rt_stub;
				str[j][0] = 0;
			}

			for(l=0; l<memory_index; l++)
			{
				if(memory_address[l] && memory_address[l] != extra_arg.dummy_node)
				{
					//printf("%d: %s(%d)\n", l, ((M_rt_stub*)memory_address[l])->skey, ((M_rt_stub*)memory_address[l])->skey_len);
					UT_ASSERT(((M_rt_stub*)memory_address[l])->skey 
						|| (!((M_rt_stub*)memory_address[l])->skey && !((M_rt_stub*)memory_address[l])->parent));
				}
			}
		}

#if 0
		for(k=0; k<STRING_NUM; k++)
		{
			printf("%s\n", str[k]);
			if(str[k][0])
			{
				search_result = &(nodes[k]->rt_stub);
				printf("node %s(%d)", search_result->skey, search_result->skey_len);
				search_result = search_result->parent;
				while(search_result)
				{
					printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
				}
				printf("\n");
			}
		}
		printf("\n");
#endif

		rm_matafy(&root, rm_alloc, &pool, &tmp_sp);

		if(extra_arg.extra_node)
			reg_mem_free(extra_arg.extra_node);
		if(!extra_arg.dummy_node)
			extra_alloc = 1;
		rm_process_arg(&pool, &extra_arg);
		if(extra_alloc)
			reg_mem_alloc(extra_arg.dummy_node);

		extra_alloc = 0;
#if 0
		for(k=0; k<STRING_NUM; k++)
		{
			printf("%s\n", str[k]);
			if(str[k][0])
			{
				search_result = &(nodes[k]->rt_stub);
				printf("node %s(%d)", search_result->skey, search_result->skey_len);
				search_result = search_result->parent;
				while(search_result)
				{
					printf(" -> %s(%d)", search_result->skey, search_result->skey_len);
					search_result = search_result->parent;
				}
				printf("\n");
			}
		}
		printf("\n");
#endif

		for(k=0; k<STRING_NUM; k++)
		{
			if(str[k][0])
			{
				search_result = rmt_search(&root, str[k], strlen(str[k])/2, RT_MODE_EXACT, &matched_len);
				UT_ASSERT(search_result);
			}
		}
		
		j = rand() % STRING_NUM;
		//j = STRING_NUM;
		//j = 2;
		fprintf(fp, "random j is %d\n", j);
		for(k=0; k<j; k++)
		{
			for(l=0; l<memory_index; l++)
			{
				if(memory_address[l] && memory_address[l] != extra_arg.dummy_node)
				{
					//printf("%d: %s(%d)\n", l, ((M_rt_stub*)memory_address[l])->skey, ((M_rt_stub*)memory_address[l])->skey_len);
					UT_ASSERT(((M_rt_stub*)memory_address[l])->skey 
						|| (!((M_rt_stub*)memory_address[l])->skey && !((M_rt_stub*)memory_address[l])->parent));
				}
			}
		}

		//rt_free_all(&root, rt_stub_free, NULL);
		//rmt_free_all_p(&root, &pool);

		if(extra_arg.dummy_node)
			reg_mem_free(extra_arg.dummy_node);
		rm_free(extra_arg.dummy_node, &pool);
		extra_arg.dummy_node = NULL;

		while(dup_nodes)
		{
			dup_tmp = dup_nodes;
			dup_nodes = dup_nodes->parent;
			reg_mem_free(dup_tmp);
			rm_free(dup_tmp, &pool);
		}

		//UT_ASSERT(memory_blocks == 0);
		//printf("\n");

	}

	rm_handle_insert_pattern(handle, &root, "10");
	rm_handle_insert_pattern(handle, &root, "20");
	rm_handle_insert_pattern(handle, &root, "30");
	//rm_handle_insert_pattern(&handle, &root, "30");
	rm_handle_insert_pattern(handle, &root, "40");
	rm_match(&root, handle);
	
	rm_parse_result(&root, handle, &match_res);
	rm_print_result(&root, handle, &match_res, get_rule, stdout);

	//rm_parse_total_result(&root, handle, &match_total_res);
	//rm_print_result(&root, handle, &match_total_res, get_rule, stdout);

	c2 = clock();
	printf("\nneed %d ms\n", c2 - c1);
	
	fclose(fp);
	rm_destroy_pool(&pool);
	free(str);
	free(nodes);
	free(mem_chunk);
	free(handle_memory);

	memory_blocks = 0;

	UT_CONCLUDE(ret);
	return ret;
}