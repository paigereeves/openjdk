#!/bin/bash
set -e

# The directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# The directory of the binding
BINDING_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
BINDING_OPENJDK_DIR="$BINDING_DIR/openjdk"
PROFILE_DATA_DIR="/tmp/$USER/pgo-data"

DO_PGO=false

make clean CONF=linux-x86_64-server-release
sh configure --disable-warnings-as-errors --with-debug-level=release --with-native-debug-symbols=internal # --with-extra-ldflags="-rdynamic -Wl,--export-dynamic-symbol=harness_begin_openjdk"

if [ $DO_PGO = true ]; then
    make CONF=linux-x86_64-server-release images JVM_PGO_PHASE=generate JVM_PGO_DIR=$PROFILE_DATA_DIR
    rm -rf $PROFILE_DATA_DIR/*
    ./build/linux-x86_64-server-release/jdk/bin/javac benchmarks/GCBench.java
    LD_PRELOAD=$HOME/distillation/libperf_statistics.so ALLOC_SAMPLE_INTERVAL=100000000 build/linux-x86_64-server-release/jdk/bin/java -server --add-exports java.base/jdk.internal.ref=ALL-UNNAMED -XX:+ExitOnOutOfMemoryError -agentpath:/home/paiger/distillation/libperf_statistics.so -Djava.library.path=/home/paiger/distillation -XX:+UseG1GC -Xms30000M -Xmx30000M -cp $PWD/benchmarks:/home/paiger/distillation GCBench
    echo "Done taking profile to dir: $PROFILE_DATA_DIR"
    make CONF=linux-x86_64-server-release images JVM_PGO_PHASE=use JVM_PGO_DIR=$PROFILE_DATA_DIR
else
    make CONF=linux-x86_64-server-release images JVM_PGO_PHASE=none
fi


if [ -e "../objdump/"$1".txt" ]; then
    mv ../objdump/"$1".txt ../objdump/"$1".txt.old
fi
echo "rsync -a build/linux-x86_64-server-release paiger@lynx.moma:~/build/$1/"
objdump -dSCl build/linux-x86_64-server-release/jdk/lib/server/libjvm.so &> ../objdump/"$1".txt
echo "Run following commands to push to lynx"
echo "rsync ../objdump/$1.txt paiger@lynx.moma:~/objdump/"