#!/bin/bash
export PATH=/opt/watcom/binl64:$PATH

set -ex
wasm wrapper.asm
wpp -0 -os -ms \
  -we \
  -zl -xd main.cpp # cpp
wlink @main.lnk
./pad_bootloader.py
