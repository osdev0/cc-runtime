#! /bin/sh

set -ex

cat >cc-runtime.c <<'EOF'
#pragma GCC diagnostic ignored "-Wunused-function"

EOF

cat assembly.h int_endianness.h int_types.h int_lib.h int_util.h \
    $(find . -name '*.c' | grep -v 'cc-runtime.c' | LC_ALL=C sort) >> cc-runtime.c

for f in *.inc; do
    sed -e "/#include \"$f\"/{r $f" -e "d}" cc-runtime.c > cc-runtime.c.tmp
    mv cc-runtime.c.tmp cc-runtime.c
done

grep '#include <' cc-runtime.c > saved-includes
sed '/#include/d' cc-runtime.c > cc-runtime.c.tmp
mv cc-runtime.c.tmp cc-runtime.c
cat saved-includes cc-runtime.c > cc-runtime.c.tmp
mv cc-runtime.c.tmp cc-runtime.c
rm -f saved-includes

awk '{for(x=1;x<=NF;x++)if($x~/_EXTERNAL_COUNTER/){sub(/_EXTERNAL_COUNTER/,++i)}}1' cc-runtime.c > cc-runtime.c.tmp
mv cc-runtime.c.tmp cc-runtime.c
