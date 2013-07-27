#!/bin/sh

# p1: version of normalize engine, x.x.x
# p2: release/debug

p1=$1;
p2=$2;

if [ -z "$p1" ] ; then
    echo "version x.x.x is needed!"
    echo "format: ./bmake.sh x.x.x debug/release"
    exit
fi;

debug_flag="-D_DEBUG -D_DEBUG_PRINT -IM_include -D__M_CFG_OS64 -D__M_CFG_OS_LINUX -Wall -g";
release_flag="-IM_include -D__M_CFG_OS64 -D__M_CFG_OS_LINUX -Wall -O2";

if [ $p2 == "release" ] ; then
    flag=$release_flag;
    p1="${p1}.r";
else
    flag=$debug_flag;
    p1="${p1}.d";
fi;



gcc -c MBase/base_struct/*.c $flag
gcc -c MBase/error/*.c $flag
gcc -c normalize_engine/ne/*.c $flag

mv MBase/base_struct/*.o .
mv MBase/error/*.o .
mv normalize_engine/ne/*.o .

ar -q libne.$p1.a *.o
rm -f *.o

gcc  normalize_engine/ne_test/*.c -o ne -lne.$p1 $flag -L.

