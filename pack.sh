#! /bin/sh

set -ex

cat >src/cc-runtime.c <<'EOF'
#pragma GCC diagnostic ignored "-Wunused-function"

EOF

cat src/assembly.h src/int_endianness.h src/int_types.h src/int_lib.h src/int_util.h \
    $(find src -name '*.c' | grep -v 'cc-runtime\.c' | LC_ALL=C sort) >> src/cc-runtime.c

for f in $(cd src && echo *.inc); do
    sed -e "/#include \"$f\"/{r src/$f" -e "d}" src/cc-runtime.c > src/cc-runtime.c.tmp
    mv src/cc-runtime.c.tmp src/cc-runtime.c
done

grep '#include <' src/cc-runtime.c > saved-includes
sed '/#include/d' src/cc-runtime.c > src/cc-runtime.c.tmp
mv src/cc-runtime.c.tmp src/cc-runtime.c
cat saved-includes src/cc-runtime.c > src/cc-runtime.c.tmp
mv src/cc-runtime.c.tmp src/cc-runtime.c
rm -f saved-includes

awk '{for(x=1;x<=NF;x++)if($x~/_EXTERNAL_COUNTER/){sub(/_EXTERNAL_COUNTER/,++i)}}1' src/cc-runtime.c > src/cc-runtime.c.tmp
mv src/cc-runtime.c.tmp src/cc-runtime.c
