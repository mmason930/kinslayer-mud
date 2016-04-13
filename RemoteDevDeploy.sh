#!/bin/bash

(

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

rsync -avz --exclude="*.o" --exclude="*.obj" src/ kin:/kinslayer/kmud-test/src/
ssh kin 'cd /kinslayer/kmud-test/src && make -j12'

)
