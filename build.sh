#!/bin/bash
export PATH=/opt/watcom/binl64:$PATH

set -ex
wasm wrapper.asm
wcc -0 -ms main.c
wlink @main.lnk
