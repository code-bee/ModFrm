// normalize_engine.cpp : 定义控制台应用程序的入口点。
//
#include "normalize_engine.h"


//ne_cfg_t	g_cfg;

int main(int argc, char* argv[])
{
	ne_cfg_t cfg;
	read_ne_config("..\\normalize_engine.cfg", &cfg);
	//read_ne_config("..\\a.txt");
	print_cfg(&cfg);
	release_ne_config(&cfg);
	return 0;
}

