#!/bin/bash

SSH_DIR_PATH="/root/.ssh";
SSH_CONFIG_FILE_PATH="$SSH_DIR_PATH/config";
SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH="$SSH_DIR_PATH/id_rsa_kinslayer_mud";
GIT_HOSTNAME=`git -C /kinslayer/ remote -v | head -1 | awk -F'@' '{print $2}' | awk -F':' '{print $1}'`

mkdir -p "$SSH_DIR_PATH"

rm -f "$SSH_CONFIG_FILE_PATH";
rm -f "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH";

echo "Host $GIT_HOSTNAME" >> "$SSH_CONFIG_FILE_PATH"
echo "Hostname github.com" >> "$SSH_CONFIG_FILE_PATH"
echo "IdentityFile=/root/.ssh/id_rsa_kinslayer_mud" >> "$SSH_CONFIG_FILE_PATH"

echo "$GIT_SSH_KEY" > "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"

chmod 700 "$SSH_DIR_PATH"
chmod 600 "$SSH_CONFIG_FILE_PATH"
chmod 600 "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"
ssh-keyscan github.com> >> "$SSH_DIR_PATH""/known_hosts"


TARGET="$1"
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
	make -C /kinslayer/src -j"$GCC_THREADS"
	echo "Starting MUD..."
	ldconfig
	./bin/kinslayer
elif [[ "$TARGET" == "gateway" ]]; then
	echo "Building gateway..."
	make -C /kinslayer/src -j"$GCC_THREADS"
	make gateway -C /kinslayer/src -j"$GCC_THREADS"
	echo "Starting gateway..."
	ldconfig
	./bin/gateway
else
	echo "Invalid target."
	exit
fi
