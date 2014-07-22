#!/bin/bash

BASEPATH=$1
DIRNAME=$2

BASEPATH=${BASEPATH%/}

echo "Basepath: $BASEPATH"

if [[ -z $BASEPATH || -z $DIRNAME ]]; then
	echo "Syntax: Switch.sh <base path> <directory name>"
	exit 1
fi

if [[ ! -d $BASEPATH ]]; then
	echo 'The base path `'$BASEPATH'` does not exist.'
	exit 1
fi

SYMLINK_DESTINATION=`find /cygdrive/c/dev/kinslayer/ -maxdepth 1 -type d -name "$DIRNAME"`

echo "Symlink Dest: $SYMLINK_DESTINATION"

if [[ -z $SYMLINK_DESTINATION || ! -d $SYMLINK_DESTINATION ]]; then
	echo "The directory '$DIRNAME' does not exist in '$BASEPATH'"
	exit 1
fi

echo "Removing old symlink."
find $BASEPATH -maxdepth 1 -mindepth 1 -type l -name "working-copy" -print0 | xargs -0 rm -f

echo "Creating new symlink."
ln -s $SYMLINK_DESTINATION $BASEPATH/working-copy