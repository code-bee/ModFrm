#ifndef __NORMALIZE_ENGINE_H__
#define __NORMALIZE_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MBase.h"

#include "config.h"

/*
	整体流程
	1. 读入配置
	2. 解析配置，生成便于使用，高效的数据结构
	3. 解析规则，生成AC状态机和规则匹配器
	4. 对输入串进行匹配
*/

/*
	1. 读入配置
*/
#define NR_GROUPS		8
#define	FLAG_LEN		32
#define RULE_LEN		256

typedef struct st_cfg_common
{
	int nr_groups_in_rule;
	int	pattern_id_len;
	char default_group[FLAG_LEN];
	char group_order[FLAG_LEN*NR_GROUPS];
} cfg_common_t;

typedef struct st_cfg_group
{
	char name[FLAG_LEN];
	char start_flag[FLAG_LEN];
	char end_flag[FLAG_LEN];
	char seg_delim[FLAG_LEN];
	int  seg_in_order;
} cfg_group_t;

typedef struct st_cfg_rule
{
	char match_rule[RULE_LEN];
	char normal_rule[RULE_LEN];
} cfg_rule_t;

/*
	用user_config_set_DECLARE和user_config_set_t定义整体配置变量都可以
	前者的不方便在于变量名由系统给出，不直观
	后者的不方便在于使用时需要做强制类型转换，更麻烦
	相比较之下，推荐前者

	更好的方法是用户自己按照宏展开后的顺序定义变量，这样变量名用户可以自己定义，
	同时沿用第二种方案中的g_config中第二项的写法即可
*/
typedef struct st_ne_cfg
{
	user_config_set_DECLARE(cfg_common_t);
	user_config_set_DECLARE(cfg_group_t);
	user_config_set_DECLARE(cfg_rule_t);
	//宏展开后等价于：
	//cfg_common_t*	common_t_cfgs;
	//int		common_t_nr_sets;
	//cfg_group_t*	group_t_cfgs;
	//int		group_t_nr_sets;
	//cfg_rule_t*	rule_t_cfgs;
	//int		rule_t_nr_sets;

	//另一种定义方式：
	//user_config_set_t common;
	//user_config_set_t group;
	//user_config_set_t rule;
} ne_cfg_t;

int read_ne_config(char* filename, ne_cfg_t* cfgs);
void release_ne_config(ne_cfg_t* cfgs);
void print_cfg(ne_cfg_t* cfgs);

/*
	2. 解析配置，生成便于使用，高效的数据结构

	a. 分析common组，然后用common组的信息解析group组
		这里需要对各种边界字符进行转义：\t,\r等等
	b. 对rule组的解析放在第3步做

	具体做法：
	将所有的组分隔符和段分隔符组织成一个AC，
	规则串、归一串、待处理串首先都需要通过这个AC得到各个分隔符的位置

	然后利用组规则、段规则解释这些分隔符，确定各个组、段的边界

	组分隔符，段分隔符的两个约束：
	1. 组分隔符与组内段分隔符必不相同
	2. 除了1，分隔符可以相同，但不能存在嵌套关系

	解析AC匹配结果时首先确定各组的边界，然后在组边界内部找段边界。组边界内部出现的非本组的段分隔符直接忽略

*/

/*
	group_t按照配置定义的group顺序组织成链表，同时在链表头上提供default_group的指针
*/
typedef struct st_cfg_group group_t;

typedef struct st_group_head
{
	M_slist		list_head;
	group_t*	default_group;
} group_head_t;

/*
	type: 记录该分隔符在对应grp中的类型：start/end/share，各占1位
*/
#define	DSTART	0x01
#define DEND	0x02
#define DSHARE	0x04
typedef struct st_group_info
{
	M_slist		list_stub;
	group_t*	grp;
	int			type;
} group_info_t;


typedef struct st_seg_info
{
	M_slist		list_stub;
	group_t*	grp;
} seg_info_t;

typedef struct st_delim
{
	//M_rdx_tree	rdx_stub;
	char		delim[FLAG_LEN];
	M_slist		grp_list;
	M_slist		seg_list;
} delim_t;

//typedef struct st_delim_info
//{} delim_info_t;

//int	build_delim_info(ne_cfg_t* cfg, delim_info_t* dinfo);
//void free_delim_info(delim_info_t* dinfo);

#endif //__NORMALIZE_ENGINE_H__


