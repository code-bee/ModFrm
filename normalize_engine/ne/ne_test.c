// normalize_engine.cpp : 定义控制台应用程序的入口点。
//
#include "normalize_engine.h"


ne_cfg_t	g_cfg;

int main(int argc, char* argv[])
{
	read_ne_config("..\\normalize_engine.cfg", &g_cfg);
	//read_ne_config("..\\a.txt");
	print_cfg(&g_cfg);
	release_ne_config(&g_cfg);
	return 0;
}

