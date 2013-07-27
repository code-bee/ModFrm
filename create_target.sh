#!/bin/sh

rm -rf target
mkdir target

./bmake.sh 1.0.0 debug

mv *.a target/
cp -r M_include/ target/
cp normalize_engine/ne/*.h target/
cp normalize_engine/ne_test/*.c target/
cp *.cfg target/
cp target_make.sh target/
chmod +x target/target_make.sh
rm target/normalize_engine_priv.h -f

sed 's#\.\./ne/normalize_engine\.h#normalize_engine\.h#g' -i target/ne_test.c
