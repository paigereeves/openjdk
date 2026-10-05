#!/bin/bash
set -e

echo $1 $2
make clean CONF=linux-x86_64-server-release
# make CONF=linux-x86_64-server-release images THIRD_PARTY_HEAP=$PWD/../mmtk-openjdk/openjdk
bash ../mmtk-openjdk/.github/scripts/pgo-build.sh $1
if [ -e "../objdump/"$2".txt" ]; then
    mv ../objdump/"$2".txt ../objdump/"$2".txt.old
fi
objdump -dSCl build/linux-x86_64-server-release/jdk/lib/server/libmmtk_openjdk.so &> ../objdump/"$2".txt
echo "Run following commands to push to lynx"
echo "rsync -a build/linux-x86_64-server-release paiger@lynx.moma:~/build/$2/"
echo "rsync ../objdump/$2.txt paiger@lynx.moma:~/objdump/"

