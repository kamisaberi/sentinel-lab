# Resolving In-Kernel eBPF & JIT Compilation Errors

This guide addresses errors encountered when compiling `bpf/xdp_filter.c` or loading bytecode into the kernel.

---

## 1. Missing BPF Target in Clang/LLVM

### Symptom
```text
clang-16: error: unknown target triple 'bpf', did you mean 'bpfeb' or 'bpfel'?
fatal error: 'bpf/bpf_helpers.h' file not found
```

### Remediation
Install the complete LLVM toolchain and libbpf development headers:

```bash
sudo apt-get install -y clang-16 llvm-16 libbpf-dev linux-headers-$(uname -r)
```

Verify that Clang lists `bpf` as a registered target:

```bash
llc-16 --version | grep -i bpf
# Expected Output: bpf - BPF (host endian)
```

---

## 2. eBPF JIT Compiler Disabled

### Symptom
Testbed packet drops exhibit high latency variance ($> 12.0\,\mu\text{s}$ instead of $< 0.84\,\mu\text{s}$) because the kernel is interpreting bytecode rather than executing native machine code.

### Remediation
Enable JIT compilation permanently:

```bash
echo "net.core.bpf_jit_enable = 1" | sudo tee -a /etc/sysctl.d/99-bpf.conf
sudo sysctl -p /etc/sysctl.d/99-bpf.conf
```

---

## 3. Missing Kernel BTF Debug Symbols (`/sys/kernel/btf/vmlinux`)

### Symptom
```text
libbpf: failed to find valid kernel BTF: No such file or directory
Error: failed to load BPF object file: -2
```

### Remediation
Ensure your kernel was compiled with `CONFIG_DEBUG_INFO_BTF=y`. On Ubuntu/Debian systems missing symbols, install the matching debug kernel image:

```bash
sudo apt-get install -y linux-image-$(uname -r)-dbg
```

