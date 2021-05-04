#!/bin/bash

TARGET=$1
echo "TARGET: $TARGET"

cd /kinslayer
mkdir -p lib/misc

chmod ug+x /kinslayer/CleanUpBenchmarks.sh /kinslayer/ImportPlayerLogs /kinslayer/PullScripts.sh
rm -f /kinslayer/lib/scripts && ln -s /kinslayer/scripts /kinslayer/lib/scripts
make clean -C /kinslayer/src

cp BasicConfig.template lib/misc/BasicConfig

sed -i 's|${DB_USERNAME}|'"$DB_USERNAME"'|' lib/misc/BasicConfig
sed -i 's|${DB_PASSWORD}|'"$DB_PASSWORD"'|' lib/misc/BasicConfig
sed -i 's|${DB_SCHEMA}|'"$DB_SCHEMA"'|' lib/misc/BasicConfig
sed -i 's|${DB_HOSTNAME}|'"$DB_HOSTNAME"'|' lib/misc/BasicConfig

if [[ "$TARGET" == "kinslayer" ]]; then
	echo "Building MUD..."
	make -C /kinslayer/src -j12
	echo "Starting MUD..."
	ldconfig
	./bin/kinslayer
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
