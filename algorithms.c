#include "radix_tree.h"
#include "radix_tree_array.h"
#include "red_black_tree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char* argv[])
{
	{
		//int i;
		//st_rbt_node* root = NULL;
		//st_rbt_node node[20];

		//unsigned short q = 255;
		//short t = -1;
		////st_rbt_node* x;

		//unsigned short x  = q+t;

		//for(i=0; i<20; i++)
		//{
		//	memset(&node[i], 0, sizeof(st_rbt_node));
		//	node[i].key = i;
		//}

		//for(i=3; i<18; i++)
		//	rbt_insert(&root, &node[i], CMP_DEFAULT_INT);

		//rbt_insert(&root, &node[2], CMP_DEFAULT_INT);
		//rbt_insert(&root, &node[0], CMP_DEFAULT_INT);
		//rbt_insert(&root, &node[19], CMP_DEFAULT_INT);
		//rbt_insert(&root, &node[1], CMP_DEFAULT_INT);
		//rbt_insert(&root, &node[18], CMP_DEFAULT_INT);



		////x = rbt_remove_key(&root, 10, CMP_DEFAULT_INT);
		//rbt_remove_key(&root, 6, CMP_DEFAULT_INT);
		//rbt_remove_key(&root, 16, CMP_DEFAULT_INT);
		//for(i=0; i<6; i++)
		//	rbt_remove_key(&root, i, CMP_DEFAULT_INT);
		//for(i=15;i>6; i--)
		//	rbt_remove_key(&root, i, CMP_DEFAULT_INT);
		//for(i=3; i<16; i++)
		//	rbt_insert(&root, &node[i], CMP_DEFAULT_INT);
		//for(i=3;i<20; i++)
		//	rbt_remove_key(&root, i, CMP_DEFAULT_INT);
		//printf("%d\n", sizeof(st_rbt_node));
		//return 0;
	}
	{
		int length = 12;
		int i = 0;
		sstring_t x, y, z, w,a, b, c, d, e, f, g, h;
		st_rt_node* root = NULL;
		st_rt_node* p = NULL;

		a.string = malloc_d(2);
		sprintf(a.string, "%s", "a");
		a.length = strlen(a.string);
		rt_insert_array_branch_char(&root, &a);
		
		b.string = malloc_d(3);
		sprintf(b.string, "%s", "bb");
		b.length = strlen(b.string);
		rt_insert_array_branch_char(&root, &b);

		c.string = malloc_d(4);
		sprintf(c.string, "%s", "bba");
		c.length = strlen(c.string);
		rt_insert_array_branch_char(&root, &c);

		d.string = malloc_d(4);
		sprintf(d.string, "%s", "bbb");
		d.length = strlen(d.string);
		rt_insert_array_branch_char(&root, &d);

		e.string = malloc_d(3);
		sprintf(e.string, "%s", "aa");
		e.length = strlen(e.string);
		rt_insert_array_branch_char(&root, &e);

		f.string = malloc_d(4);
		sprintf(f.string, "%s", "aaa");
		f.length = strlen(f.string);
		rt_insert_array_branch_char(&root, &f);

		g.string = malloc_d(4);
		sprintf(g.string, "%s", "aaa");
		g.length = strlen(g.string);
		rt_insert_array_branch_char(&root, &g);

		h.string = malloc_d(3);
		sprintf(h.string, "%s", "ab");
		h.length = strlen(h.string);
		rt_insert_array_branch_char(&root, &h);

		x.string = malloc_d(4);
		sprintf(x.string, "%s", "aab");
		x.length = strlen(x.string);
		rt_insert_array_branch_char(&root, &x);

		y.string = malloc_d(4);
		sprintf(y.string, "%s", "abb");
		y.length = strlen(y.string);
		rt_insert_array_branch_char(&root, &y);

		z.string = malloc_d(2);
		sprintf(z.string, "%s", "b");
		z.length = strlen(z.string);
		rt_insert_array_branch_char(&root, &z);

		w.string = malloc_d(3);
		sprintf(w.string, "%s", "ba");
		w.length = strlen(w.string);
		rt_insert_array_branch_char(&root, &w);	

		
		//p = rt_remove_array_branch_char(&root, &g);
		//p = rt_remove_array_branch_char(&root, &x);
		//p = rt_remove_array_branch_char(&root, &y);
		//p = rt_remove_array_branch_char(&root, &w);
		//p = rt_remove_array_branch_char(&root, &c);
		//p = rt_remove_array_branch_char(&root, &d);
		//p = rt_remove_array_branch_char(&root, &b);
		//p = rt_remove_array_branch_char(&root, &e);
		//p = rt_remove_array_branch_char(&root, &h);
		//p = rt_remove_array_branch_char(&root, &a);
		//p = rt_remove_array_branch_char(&root, &z);

		rt_free_all_array_branch_char(&root);

	
		return 0;
	}
}