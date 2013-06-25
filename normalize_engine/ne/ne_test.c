// normalize_engine.cpp : 定义控制台应用程序的入口点。
//

#if (defined _WINDOWS) || (defined WIN32)
#ifdef _DEBUG
	#pragma comment(lib, "../debug/MBase.lib")
	#pragma comment(lib, "../debug/pthread.lib")
#else
	#pragma comment(lib, "../release/MBase.lib")
	#pragma comment(lib, "../release/pthread.lib")
#endif
#endif

#include "normalize_engine.h"

#define MEM_LEAK
//ne_cfg_t	g_cfg;

int main(int argc, char* argv[])
{
	ne_cfg_t	cfg;
	normalize_engine_t	model;
	M_sint8*	key = "/";
	M_sint32	key_len = 0;
	M_sint32	match_len;
	M_rt_stub*	rt_stub;
	pattern_t*	pat;
	M_slist*	list_stub;
	delim_cons_t*	delim_cons;
	M_sint32	i;
	read_ne_config("..\\normalize_engine\\ne\\normalize_engine.cfg", &cfg);
	//read_ne_config("..\\normalize_engine\\ne\\a.txt", &cfg);
	print_cfg(&cfg);

	for(i = 0; i<1; i++)
	{
		i = build_normalize_engine(&model, &cfg, 1024*1024);
		rt_stub = rt_search(model.pat_tree, key, key_len, RT_MODE_EXACT, &match_len);
		pat = container_of(rt_stub, pattern_t, rt_stub);
		list_stub = pat->cons_head.next;

		while(list_stub != &pat->cons_head)
		{
			delim_cons = container_of(list_stub, delim_cons_t, pat_stub);
			list_stub = list_stub->next;
		}

		destroy_normalize_engine(&model);
		printf("%d\n", i);
	}
	match_len = pi_get_mem_cts();
	release_ne_config(&cfg);
	return 0;
}

