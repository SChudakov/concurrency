#!/usr/bin/env bash

gcc -o ./exec/process process.c
gcc -o ./exec/memory_read memory_read.c
gcc -o ./exec/keystroke_capture keystroke_capture.c
gcc -o ./exec/disable_cache disable_cache.c
