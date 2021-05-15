#!/bin/bash


mkdir -p /root/.ssh

GIT_HOSTNAME=`git -C /kinslayer/ remote -v | head -1 | awk -F'@' '{print $2}' | awk -F':' '{print $1}'`

echo "Host $GIT_HOSTNAME" >> /root/.ssh/config
echo "Hostname github.com" >> /root/.ssh/config
echo "IdentityFile=/root/.ssh/id_rsa_kinslayer_mud" >> /root/.ssh/config

echo "ssh-rsa $GIT_SSH_KEY root@container" > /root/.ssh/id_rsa.pub

chmod 700 /root/.ssh
chmod 600 /root/.ssh/config
chmod 600 /root/.ssh/id_rsa.pub


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
