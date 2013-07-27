#ifndef __NORMALIZE_ENGINE_H__
#define __NORMALIZE_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MBase.h>

#include "config.h"
#include "acsmx2.h"

#ifdef _WINDOWS
#define	__M_CFG_OS_WINDOWS
#endif

#ifdef WIN32
#define	__M_CFG_OS_WINDOWS
#endif

#ifdef __M_CFG_OS_WINDOWS

	#ifdef NORMALIZE_ENGINE_EXPORTS
		#define NE_API __declspec(dllexport)
	#else
		#define NE_API __declspec(dllimport)
	#endif

	//#define __M_CFG_CMPL_MVC

#else

	#define NE_API

#endif

/*
	整体流程
	1. 读入配置
	2. 解析配置，生成便于使用，高效的数据结构
	3. 对输入串进行匹配，提取通配字段，构造归一化结果
*/

/*
	1. 读入配置
*/
#define NR_GROUPS		8
#define GROUP_NAME_LEN	16
#define	FLAG_LEN		16
#define RULE_LEN		256

#define	WT_SINGLECHAR	0
#define	WT_SINGLESEG	1
#define	WT_MULTICHAR	2
#define	WT_MULTISEG		3
#define	NR_WILDCARD		4

typedef struct st_cfg_common
{
	M_sint8		default_group[GROUP_NAME_LEN];
	M_sint8		group_order[GROUP_NAME_LEN*NR_GROUPS];
	M_sint8		wildcard[NR_WILDCARD][FLAG_LEN];
	M_sint8		pattern_id_len;
	M_sint8		escape_char;
	M_sint8		nr_groups;
	M_sint8		group_delim_reuse;
	M_sint8		flow_mode;			
} cfg_common_t;

typedef struct st_cfg_group
{
	M_sint8 name[FLAG_LEN];
	M_sint8 start_flag[FLAG_LEN];
	M_sint8 end_flag[FLAG_LEN];
	M_sint8 seg_delim[FLAG_LEN];
	M_sint32  seg_in_order;
} cfg_group_t;

typedef struct st_cfg_rule
{
	M_sint8 match_rule[RULE_LEN];
	M_sint8 normal_rule[RULE_LEN];
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
	//cfg_common_t*	cfg_common_t_cfgs;
	//M_sint32			cfg_common_t_nr_sets;
	//cfg_group_t*	cfg_group_t_cfgs;
	//M_sint32			cfg_group_t_nr_sets;
	//cfg_rule_t*	cfg_rule_t_cfgs;
	//M_sint32			cfg_rule_t_nr_sets;

	//另一种定义方式：
	//user_config_set_t common;
	//user_config_set_t group;
	//user_config_set_t rule;
} ne_cfg_t;

NE_API M_sint32 read_ne_config(M_sint8* filename, ne_cfg_t* cfgs);
NE_API void release_ne_config(ne_cfg_t* cfgs);
NE_API void print_cfg(ne_cfg_t* cfgs);

typedef cfg_group_t	group_t;

typedef struct st_pattern
{
	//M_rt_stub	rt_stub;
	M_sint8*	str;
	M_sint16*	delim_type;		//not NULL if type == PT_DELIM
								//delim type array, each group has an entry corresponding to group id
								//DT_XXX, only DT_START/DT_END/DT_SEG is valid in pattern_t, 
								//DT_HAT/DT_DOLLAR does not exist in pattern_t
	M_sint16	str_len;
	M_sint16	type;			//PT_XXX, string pattern or delimiter pattern
	
	//delim_info_t	delim_info;	//delimiter info. only valid if type == PT_DELIM
} pattern_t;

/*
	each wildcard in match rule is descripted by a wc_info_t structure
	because wildcard absorbs 1 delimiter at most, the delimiter it absorbs also list here
*/
struct st_rm_wc_info;
typedef struct st_wc_info
{
	struct st_rm_wc_info*	rm_wc_info;
	pattern_t*		wc_pat;
	M_sint32		pos;			//char position relative to whole match rule
	M_sint16		seg_pos;		//seg positions relative to current group	
	M_sint8			wc_seq;			//sequence id of wildcard in match_rule
	M_sint8			nm_wc_seq;		//sequence id of wildcard in normal_rule
	M_sint8			grp;			//which group current wildcard belongs to
	M_sint8			cmp_type;		//CT_XXX, start pos is greater or equle to start_pos
	M_sint8			wc_type;		//WT_XXX, single char, single seg, multi char, multi seg...
	M_sint8			wc_len;			//length of current wildcard, including sequence id
} wc_info_t;

typedef struct st_delim_pos
{
	M_bst_stub	rbt_stub;
	M_dlist		list_stub;		//used by mat_delim_t->delim_head
	pattern_t*	delim_pat;		//delimiter info, lays in pat_tree
	M_sint32	pos;			//char pos, key
	M_sint16	seg_pos;		//seg pos relative to current group
	M_sint8		grp_id;			//index of group that it belongs to
	M_sint8		color;
} delim_pos_t;

typedef struct st_delim_info
{
	pattern_t*	delim_pat;
	M_sint32	pos;			//char pos, key
	M_sint16	seg_pos;		//seg pos relative to current group
	M_sint8		grp_id;			//index of group that it belongs to
} delim_info_t;

typedef struct st_rm_wc_info
{
	M_sint16		nr_wcs;			// number of wildcards in wc_info
	M_sint16		nr_delims;
	wc_info_t*		wc_info;		// no memory allocated, points to some offset of rule_t->ori_wc_arr
	delim_info_t*	delim_info;		// dynamic allocated memory from ne_arg->spool
} rm_wc_info_t;

typedef struct st_normal_info
{
	M_slist		nm_stub;		//used by rule_t->nm_head
	M_sint8*	str;			//string segment, part of rule_t->normal_rule
	M_sint32	str_len;		//length of string segment
	wc_info_t*	next_wc;		//nm_wc just after str. if normal_info is lead by wildcard, str of first normal_info is NULL
} normal_info_t;

typedef struct st_rule
{
	M_sint32		nr_ori_wc;		//number of wildcards in match_rule
	M_sint32		nr_rm_wc;		//number of wildcards in rm_rule
	M_sint32		leading_grp;	//group id of first group
	wc_info_t*		ori_wc_arr;		//array of ori wildcard, has nr_ori_wc entries
	rm_wc_info_t*	rm_wc_arr;		//wc array, has nr_rm_wc entries, for looking nr_wc up in match_rules
	M_slist			nm_head;		//info of normal rule
	M_sint8*		match_rule;		//match rule string
	M_sint8*		normal_rule;	//normal rule string
} rule_t;

typedef struct st_acmata
{
	ACSM_STRUCT2*	ac_handle;
	//callbacks...
} acmata_t;

/*
	top structure, combine all data together
	normalize_engine_t is a read only structure, could be shared among threads
*/
typedef struct st_normalize_engine
{
	acmata_t	acmata;			//ac mata
	M_rm_root	rm_tree;
	M_sint8*	head_grps;		//group array, 1 means corresponding group can be head-group, 0 otherwise, only valid when flow_mode is false
	M_sint8*	tail_grps;		//group array, 1 means corresponding group can be tail-group, 0 otherwise. 
	rule_t*		rules;			//rule array
	M_sint32	nr_rules;		//number of rules
	void*		memory;			//memory block that ne model layes in
	pattern_t**	seg_pat;		//keep seg delim pattern here. 
	M_sint8		default_grp;	//index of default group
	M_sint8		disorder_seg;	//1 if exists seg without order, 0 otherwise
	M_sint16	rm_depth;		//depth of radix mata tree
} normalize_engine_t;

/*
	called after config file read over, usually in main thread
	memory_size: in and out. [in]: how many memory will be allocated for normalize engine
							 [out]: how many memory is acturally used
	tmp_memory_size: out.	 [out]: how many temporary memory is acturally used

	size of temporary memory is same with  memory_size
*/
NE_API normalize_engine_t*	build_normalize_engine(ne_cfg_t* cfg, M_sint32* memory_size, M_sint32* tmp_memory_size);
NE_API void				destroy_normalize_engine(normalize_engine_t* model);

typedef struct st_range_result
{
	M_slist				list_stub;
	M_rm_result_node*	range_result[4];	//分别存储各个类型通配的对应范围
	rule_t*				matched_rule;
} range_result_t;

typedef struct st_match_handle
{
	// 这一组三个变量为通用变量，一次初始化，后面无需再变动
	M_stackpool tpool;			
	normalize_engine_t*	model;
	ne_cfg_t*	cfg;

	M_dlist		delim_head;		// 记录AC匹配时各分隔符的位置信息
	M_sint32	nr_delims;
	M_sint8*	back_mem;		// 记录以上变量初始化完毕后tpool的位置，便于回退。也在初始化时设定
	
	M_rm_handle*	handle;
	M_dlist			rm_result;
	delim_pos_t*	left_dummy[2];		//表示左边边界的dummy delim pos，下标决定是valid还是invalid
	delim_pos_t*	right_dummy[2];		//表示右边边界的dummy delim pos，下标决定是valid还是invalid
	M_slist		result_head;
	M_sint32	nr_matched_rules;
	M_sint32	status;
} match_handle_t;

NE_API match_handle_t*	create_match_handle(normalize_engine_t* model, ne_cfg_t* cfg, M_sint32 memory_size);
// 返回最大占用内存
NE_API M_sint32			destroy_match_handle(match_handle_t* handle);
NE_API void			set_match_handle(match_handle_t* handle, M_sint8* src_str, M_sint32 src_str_len);

/*
	返回match_handle->status
*/
NE_API M_sint32	normalize_string(match_handle_t* match_handle);
NE_API M_sint32	get_normalize_rule_count(match_handle_t* match_handle);
NE_API M_sint8*	get_normalize_string(match_handle_t* match_handle, M_sint32 rule_id, rule_t** rule, M_sint32* str_len);

#endif //__NORMALIZE_ENGINE_H__


