#!/bin/bash

qemu-system-i386 -s -S -drive format=raw,file=$1 &
pid=$!
gdb \
  -ex "target remote localhost:1234" \
  -ex "set confirm off"

kill -9 $!
