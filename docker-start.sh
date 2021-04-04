#!/bin/bash

TARGET=$1
echo "TARGET: $TARGET"

cd /kinslayer
chmod ug+x /kinslayer/CleanUpBenchmarks.sh /kinslayer/ImportPlayerLogs /kinslayer/PullScripts.sh
ln -s /kinslayer/scripts /kinslayer/lib/scripts
make clean -C /kinslayer/src

if [[ "$TARGET" == "circle" ]]; then
	echo "Building MUD..."
	make -C /kinslayer/src -j12
	echo "Starting MUD..."
	ldconfig
	./bin/circle
elif [[ "$TARGET" == "gateway" ]]; then
	echo "Building gateway..."
	make -C /kinslayer/src -j12
	make gateway -C /kinslayer/src -j12
	echo "Starting gateway..."
	ldconfig
	./bin/gateway
else
	echo "Invalid target."
	exit
fi
