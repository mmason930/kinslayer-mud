#!/bin/bash

find /kinslayer/kmud-live/lib/logs/benchmarks -maxdepth 1 -type f -mtime +2 -print0 | xargs -0 rm -f
