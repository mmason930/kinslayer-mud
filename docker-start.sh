#!/bin/bash

### Parse arguments to this script
TARGET="$1"
FULL_PARTIAL="$2"
echo "TARGET: $TARGET"
echo "FULL OR PARTIAL: $FULL_PARTIAL"

### Install the Github SSH key
#SSH_DIR_PATH="/root/.ssh";
#SSH_CONFIG_FILE_PATH="$SSH_DIR_PATH/config";
#SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH="$SSH_DIR_PATH/id_rsa_kinslayer_mud";
#SSH_KNOWN_HOSTS_FILE_PATH="$SSH_DIR_PATH/known_hosts"
#GIT_HOSTNAME=`git -C /kinslayer/ remote -v | head -1 | awk -F'@' '{print $2}' | awk -F':' '{print $1}'`

#mkdir -p "$SSH_DIR_PATH"

#rm -f "$SSH_CONFIG_FILE_PATH";
#rm -f "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH";

#echo "Host $GIT_HOSTNAME" >> "$SSH_CONFIG_FILE_PATH"
#echo "Hostname github.com" >> "$SSH_CONFIG_FILE_PATH"
#echo "IdentityFile=/root/.ssh/id_rsa_kinslayer_mud" >> "$SSH_CONFIG_FILE_PATH"

#echo "$GIT_SSH_KEY" > "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"
#sed -i -s 's/\\n/\n/g' "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"

#chmod 700 "$SSH_DIR_PATH"
#chmod 600 "$SSH_CONFIG_FILE_PATH"
#chmod 600 "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"
#ssh-keyscan github.com > "$SSH_KNOWN_HOSTS_FILE_PATH"

### Set up game directories & executable permissions
cd /kinslayer
mkdir -p lib/misc

chmod ug+x /kinslayer/CleanUpBenchmarks.sh /kinslayer/ImportPlayerLogs /kinslayer/PullScripts.sh
rm -f /kinslayer/lib/scripts && ln -s /kinslayer/scripts /kinslayer/lib/scripts
cp BasicConfig.template lib/misc/BasicConfig
sed -i 's|${DB_USERNAME}|'"$DB_USERNAME"'|' lib/misc/BasicConfig
sed -i 's|${DB_PASSWORD}|'"$DB_PASSWORD"'|' lib/misc/BasicConfig
sed -i 's|${DB_SCHEMA}|'"$DB_SCHEMA"'|' lib/misc/BasicConfig
sed -i 's|${DB_HOSTNAME}|'"$DB_HOSTNAME"'|' lib/misc/BasicConfig
if [[ ! -z "$SENDGRID_API_KEY" ]]; then
	sed -i 's|${SENDGRID_API_KEY}|'"$SENDGRID_API_KEY"'|' lib/misc/BasicConfig
fi

### Set up core dump filename pattern - the defaults on some systems (ex: Ubuntu)
### don't work for us, so we'll need to set it explicitly. Note that this operation
### does require privileged access when running the container, otherwise attepting
### to modify this file will fail.
echo "core.%p.%t" > /proc/sys/kernel/core_pattern

sleep 1000000000

### Perform clean build if specified.
if [[ "$FULL_PARTIAL" == "full" ]]; then
	make clean -C /kinslayer/src
fi

### Perform the build and run the target process
if [[ "$TARGET" == "kinslayer" ]]; then
	echo "Building MUD..."
	make -C /kinslayer/src -j"$GCC_THREADS"
	echo "Starting MUD..."
	ldconfig
	sleep 1000000000
	./bin/kinslayer
elif [[ "$TARGET" == "gateway" ]]; then
	echo "Building gateway..."
	make -C /kinslayer/src -j"$GCC_THREADS"
	make gateway -C /kinslayer/src -j"$GCC_THREADS"
	echo "Starting gateway..."
	ldconfig
	sleep 1000000000
	./bin/gateway
else
	echo "Invalid target."
	exit
fi
