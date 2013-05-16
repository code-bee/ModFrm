#ifndef __NORMALIZE_ENGINE_H__
#define __NORMALIZE_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MBase.h"

#include "config.h"

/*
	��������
	1. ��������
	2. �������ã����ɱ���ʹ�ã���Ч�����ݽṹ
	3. ������������AC״̬���͹���ƥ����
	4. �����봮����ƥ��
*/

/*
	1. ��������
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
	��user_config_set_DECLARE��user_config_set_t�����������ñ���������
	ǰ�ߵĲ��������ڱ�������ϵͳ��������ֱ��
	���ߵĲ���������ʹ��ʱ��Ҫ��ǿ������ת�������鷳
	��Ƚ�֮�£��Ƽ�ǰ��

	���õķ������û��Լ����պ�չ�����˳��������������������û������Լ����壬
	ͬʱ���õڶ��ַ����е�g_config�еڶ����д������
*/
typedef struct st_ne_cfg
{
	user_config_set_DECLARE(cfg_common_t);
	user_config_set_DECLARE(cfg_group_t);
	user_config_set_DECLARE(cfg_rule_t);
	//��չ����ȼ��ڣ�
	//cfg_common_t*	common_t_cfgs;
	//int		common_t_nr_sets;
	//cfg_group_t*	group_t_cfgs;
	//int		group_t_nr_sets;
	//cfg_rule_t*	rule_t_cfgs;
	//int		rule_t_nr_sets;

	//��һ�ֶ��巽ʽ��
	//user_config_set_t common;
	//user_config_set_t group;
	//user_config_set_t rule;
} ne_cfg_t;

int read_ne_config(char* filename, ne_cfg_t* cfgs);
void release_ne_config(ne_cfg_t* cfgs);
void print_cfg(ne_cfg_t* cfgs);

/*
	2. �������ã����ɱ���ʹ�ã���Ч�����ݽṹ

	a. ����common�飬Ȼ����common�����Ϣ����group��
		������Ҫ�Ը��ֱ߽��ַ�����ת�壺\t,\r�ȵ�
	b. ��rule��Ľ������ڵ�3����

	����������
	�����е���ָ����Ͷηָ�����֯��һ��AC��
	���򴮡���һ�������������ȶ���Ҫͨ�����AC�õ������ָ�����λ��

	Ȼ����������򡢶ι��������Щ�ָ�����ȷ�������顢�εı߽�

	��ָ������ηָ���������Լ����
	1. ��ָ��������ڶηָ����ز���ͬ
	2. ����1���ָ���������ͬ�������ܴ���Ƕ�׹�ϵ

	����ACƥ����ʱ����ȷ������ı߽磬Ȼ������߽��ڲ��Ҷα߽硣��߽��ڲ����ֵķǱ���Ķηָ���ֱ�Ӻ���

*/

/*
	group_t�������ö����group˳����֯������ͬʱ������ͷ���ṩdefault_group��ָ��
*/
typedef struct st_cfg_group group_t;

typedef struct st_group_head
{
	M_slist		list_head;
	group_t*	default_group;
} group_head_t;

/*
	type: ��¼�÷ָ����ڶ�Ӧgrp�е����ͣ�start/end/share����ռ1λ
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


