#!/bin/bash

TARGET=$1
echo "TARGET: $TARGET"

cd /kinslayer

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
