#!/bin/bash

rsync -avz --exclude="*.o" --exclude="*.obj" src/ kin:/kinslayer/kmud-test/src/
ssh kin 'cd /kinslayer/kmud-test/src && make -j12'