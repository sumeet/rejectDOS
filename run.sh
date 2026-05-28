#!/bin/bash
set -ex

GDB_FLAGS=""
if [ "$DEBUG" ]; then
    GDB_FLAGS="-s -S"
fi

# `tput reset` supposed to be faster than regular `reset`
qemu-system-i386 -cpu qemu32 -fda bootloader.bin -serial tcp:127.0.0.1:4444 $GDB_FLAGS ; tput reset
