#!/bin/bash
set -ex

cd /home/bashtv/rejectDOS

# Setup OpenWatcom paths
export WATCOM=/home/bashtv/openwatcom
export PATH=$WATCOM/binl:$PATH
export INCLUDE=$WATCOM/h

# 1. Compile Stage 1 components (C++ Logo and Wrapper)
echo "=== STEP 1: COMPILING STAGE 1 (LOGO) ==="
wasm -fo=stage1_wrapper.o stage1_wrapper.asm
wpp -0 -os -ms -we -zl -xd stage1.cpp -fo=stage1.o
wlink @stage1.lnk

# 2. Compile Stage 2 components (C Shell and Wrapper)
echo "=== STEP 2: COMPILING STAGE 2 (SHELL) ==="
wasm -fo=stage2_wrapper.o stage2_wrapper.asm
wcc -0 -os -ms -we -zl -s stage2_rust.c -fo=stage2_rust.o
wlink @stage2_rust.lnk

# 3. Generate combined multi-stage floppy disk image
echo "=== STEP 3: PACKAGING BOOT FLOPPY ==="
python3 package_floppy.py

echo "=== BUILD SUCCESSFUL: MULTI-STAGE NATIVE LOADER READY ==="
