#!/bin/bash
set -ex

export PATH=/opt/watcom/binl64:$PATH


set -ex
wasm stage1_wrapper.asm
#wcc -0 -os -ms -za99 main.c # C
wpp -0 -os -ms \
  -we \
  -zl -xd stage1.cpp # cpp
wlink @stage1.lnk
./pad_bootloader.py
