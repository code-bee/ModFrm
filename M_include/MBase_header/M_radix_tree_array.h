#ifndef _RADIX_TREE_ARRAY_H_
#define _RADIX_TREE_ARRAY_H_

#include "M_radix_tree.h"

///////////////////////////////////////////////////////////////
// type specific part
///////////////////////////////////////////////////////////////
#pragma pack(1)
typedef struct tag_array_branch
{
	st_rt_node**	array;		//pointer array
	unsigned short	length;		//array length.
	unsigned short	space;		//space to place pointers
}st_array_branch;
#pragma pack()

st_rt_node* rt_search_array_branch_char(st_rt_node* root, sstring_t* key);
st_rt_node* rt_insert_array_branch_char(st_rt_node** root, sstring_t* key);
st_rt_node* rt_remove_array_branch_char(st_rt_node** root, sstring_t* key);
void rt_free_all_array_branch_char(st_rt_node** root);

#endif // _RADIX_TREE_ARRAY_H_