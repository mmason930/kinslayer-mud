#!/bin/bash
# prune include files one at a time, recompile, and put them back if it doesn't compile
# arguments are list of files to check
function removeinclude() {
    file="$1"
    header="$2"
    perl -i -p -e 's+([ \t]*#include[ \t][ \t]*[\"\<]'$2'[\"\>])+//REMOVEINCLUDE $1+' "$1"
}
function replaceinclude() {
   file="$1"
   perl -i -p -e 's+//REMOVEINCLUDE ++' "$1"
}

for file in $*
do
    dos2unix "$file"
    includes=`grep "^[ \t]*#include" $file | awk '{print $2;}' | sed 's/[\"\<\>]//g'`
    for i in $includes
    do
        touch "$file" # just to be sure it recompiles
        echo "$file: Testing $i"
        removeinclude "$file" "$i"
        if make -j10 >/dev/null  2>&1;
        then
            grep -v REMOVEINCLUDE "$file" > tmp && mv tmp "$file"
            echo "$file: Removed $i"
        else
            replaceinclude "$file"
            echo "$file: Kept $i"
        fi
    done
    unix2dos "$file"
done
