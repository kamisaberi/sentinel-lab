#!/usr/bin/env bash
set -e

ARCH=$(uname -m)
echo "Compiling native eBPF XDP filter bytecode for: ${ARCH}..."

clang -O2 -target bpf \
      -I/usr/include/${ARCH}-linux-gnu \
      -I/usr/include \
      -c bpf/xdp_filter.c \
      -o bpf/xdp_filter.o

echo "Bytecode compiled successfully: bpf/xdp_filter.o"