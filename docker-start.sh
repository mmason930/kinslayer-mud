#!/bin/bash

### Parse arguments to this script
TARGET="$1"
FULL_PARTIAL="$2"
GCC_THREADS="${GCC_THREADS:-4}"
echo "TARGET: $TARGET"
echo "FULL OR PARTIAL: $FULL_PARTIAL"

printenv | perl -pe "s|(^.*?)=(.*$)|export \1\='\2'|" > /.env.sh
chmod +x /.env.sh
service cron start

### Install the Github SSH key
SSH_DIR_PATH="/root/.ssh";
SSH_CONFIG_FILE_PATH="$SSH_DIR_PATH/config";
SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH="$SSH_DIR_PATH/id_rsa_kinslayer_mud";
SSH_KNOWN_HOSTS_FILE_PATH="$SSH_DIR_PATH/known_hosts"
GIT_HOSTNAME=`git -C /kinslayer/ remote -v | head -1 | awk -F'@' '{print $2}' | awk -F':' '{print $1}'`

mkdir -p "$SSH_DIR_PATH"

rm -f "$SSH_CONFIG_FILE_PATH";
rm -f "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH";

echo "Host $GIT_HOSTNAME" >> "$SSH_CONFIG_FILE_PATH"
echo "Hostname github.com" >> "$SSH_CONFIG_FILE_PATH"
echo "IdentityFile=/root/.ssh/id_rsa_kinslayer_mud" >> "$SSH_CONFIG_FILE_PATH"

echo "$GIT_SSH_KEY" > "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"
sed -i -s 's/\\n/\n/g' "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"

chmod 700 "$SSH_DIR_PATH"
chmod 600 "$SSH_CONFIG_FILE_PATH"
chmod 600 "$SSH_KINSLAYER_MUD_GIT_KEY_FILE_PATH"
ssh-keyscan github.com > "$SSH_KNOWN_HOSTS_FILE_PATH"

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
sed -i 's|${RESTART_ON_SHUTDOWN}|'"${RESTART_ON_SHUTDOWN:-1}"'|' lib/misc/BasicConfig
if [[ ! -z "$SENDGRID_API_KEY" ]]; then
	sed -i 's|${SENDGRID_API_KEY}|'"$SENDGRID_API_KEY"'|' lib/misc/BasicConfig
fi
if [[ ! -z "$OPENAI_API_KEY" ]]; then
	sed -i 's|${OPENAI_API_KEY}|'"$OPENAI_API_KEY"'|' lib/misc/BasicConfig
fi

### Set up core dump filename pattern - the defaults on some systems (ex: Ubuntu)
### don't work for us, so we'll need to set it explicitly. Note that this operation
### does require privileged access when running the container, otherwise attepting
### to modify this file will fail.
echo "core.%p.%t" > /proc/sys/kernel/core_pattern

# Build failures must stop startup instead of launching a stale executable.
set -e

# Keep objects inside the container so an image upgrade cannot reuse objects
# compiled against the previous Ubuntu/Boost toolchain on the source mount.
BUILD_OBJDIR=/tmp/kinslayer-obj

### Perform clean build if specified, including in SSH/dev mode.
if [[ "$FULL_PARTIAL" == "full" ]]; then
	make clean -C /kinslayer/src OBJDIR="$BUILD_OBJDIR"
fi

### SSH/dev target: start sshd + gateway.
if [[ "$TARGET" == "ssh" ]]; then
	# With automatic MUD startup disabled, CLion controls the game build.
	# Otherwise build the game too, before the gateway can launch it.
	BUILD_TARGET=all
	if [[ "${RESTART_ON_SHUTDOWN:-1}" == "0" ]]; then
		BUILD_TARGET=gateway
	fi
	make "$BUILD_TARGET" -C /kinslayer/src -j"$GCC_THREADS" OBJDIR="$BUILD_OBJDIR"
	mkdir -p /run/sshd
	echo 'root:dev' | chpasswd
	echo "PermitRootLogin yes" >> /etc/ssh/sshd_config
	/usr/sbin/sshd

	ldconfig
	echo "Starting gateway..."
	/kinslayer/bin/gateway &

	echo "Container ready. Gateway running. SSH available on port 22."
	sleep 999999d
	exit
fi

### Perform the build and run the target process
if [[ "$TARGET" == "kinslayer" ]]; then
	echo "Building MUD..."
	make kinslayer -C /kinslayer/src -j"$GCC_THREADS" OBJDIR="$BUILD_OBJDIR"
	echo "Starting MUD..."
	ldconfig
	exec ./bin/kinslayer
elif [[ "$TARGET" == "gateway" ]]; then
	echo "Building gateway..."
	make all -C /kinslayer/src -j"$GCC_THREADS" OBJDIR="$BUILD_OBJDIR"
	echo "Starting gateway..."
	ldconfig
	exec ./bin/gateway
else
	echo "Invalid target."
	exit 1
fi
