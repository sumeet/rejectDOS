#!/bin/bash
set -ex

cd /home/bashtv/rejectDOS

# Setup OpenWatcom paths
export WATCOM=/home/bashtv/openwatcom
export PATH=$WATCOM/binl:$PATH
source "$HOME/.cargo/env"

# 1. Compile Rust Math core and translate to 16-bit NASM OMF
echo "=== STEP 1: COMPILING RUST MATH ENGINE ==="
rustc --emit=asm -O --target=i686-unknown-linux-gnu -C panic=abort rust_src/math.rs
python3 rust_src/translate.py rust_src/math.s rust_src/math.asm
nasm -f obj rust_src/math.asm -o r_math.obj

# 2. Compile Stage 1 components (C++ Logo and Wrapper)
echo "=== STEP 2: COMPILING STAGE 1 (LOGO) ==="
wasm -fo=stage1_wrapper.o stage1_wrapper.asm
wpp -0 -os -ms -we -zl -xd stage1.cpp -fo=stage1.o
wlink @stage1.lnk

# 3. Compile Stage 2 components (C Shell and Wrapper)
echo "=== STEP 3: COMPILING STAGE 2 (SHELL) ==="
wasm -fo=stage2_wrapper.o stage2_wrapper.asm
wcc -0 -os -ms -we -zl -s stage2_rust.c -fo=stage2_rust.o
wlink @stage2_rust.lnk

# 4. Generate combined multi-stage floppy disk image
echo "=== STEP 4: PACKAGING BOOT FLOPPY ==="
python3 package_floppy.py

echo "=== BUILD SUCCESSFUL: MULTI-STAGE LOADER READY ==="
