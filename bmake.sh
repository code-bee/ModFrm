#!/bin/sh

gcc -g MBase/base_struct/*.c MBase/error/*.c normalize_engine/ne/*.c -o ne -IM_include -D__M_CFG_OS64 -D__M_CFG_OS_LINUX -D__M_CFG_POOL_LEAK

