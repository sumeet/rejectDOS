#!/bin/bash
set -ex

GDB_FLAGS=""
if [ "$DEBUG" ]; then
    GDB_FLAGS="-s -S"
fi

# `tput reset` supposed to be faster than regular `reset`
qemu-system-i386 -cpu qemu32 -fda bootloader.bin $GDB_FLAGS -nographic ; tput reset
