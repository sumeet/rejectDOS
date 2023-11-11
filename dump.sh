#!/bin/bash
set -ex
objdump -M intel -D -b binary -m i8086 -Maddr16,data16 --adjust-vma=0x7C00 ./main.com
