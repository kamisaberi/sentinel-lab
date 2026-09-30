---

### File: `sentinel-lab/docs/getting-started/system-requirements.md`

```markdown
# System Requirements & Academic Lab Prerequisites

Review the toolchain, kernel configuration, and hardware requirements before building `sentinel-lab`.

---

## 1. Operating System & Toolchain

* **Operating System:** 64-bit Linux (Ubuntu 22.04 LTS, Ubuntu 24.04 LTS, Debian 12 Bookworm).
* **Linux Kernel:** Version **>= 5.15** (Kernel **>= 6.8** recommended for modern BTF and XDP features).
* **C++ Compiler:** Clang 16.0+ or GCC 12.1+ supporting **ISO C++20**.
* **eBPF Compiler:** Clang/LLVM 15.0+ with `-target bpf`.
* **Build System:** CMake **>= 3.20** and Ninja Build **>= 1.10**.
* **Python Runtime:** Python **>= 3.10** with `requests`, `numpy`, and `tqdm`.

---

## 2. Hardware Testbed Recommendations

The testbed runs in two modes:

| Resource Profile | Minimal Educational Setup (Student Laptop) | High-Performance Research Testbed (Lab Server) |
| :--- | :--- | :--- |
| **CPU** | 4-Core x86_64 or Apple Silicon (VMware/UTM) | Intel Core i9-14900K or AMD Ryzen 9 7950X |
| **RAM** | 8 GB RAM | 64 GB to 192 GB DDR5 RAM |
| **NIC** | Standard Virtual Adapter (`e1000`, `vmxnet3`) | Intel 82599ES (10GbE) or Intel E810 (25GbE) |
| **AI Acceleration** | Intel CPU AVX2 Reference Backend | Intel Core Ultra NPU / NVIDIA RTX A4000 / Jetson |

---

## 3. Required Linux Kernel Configuration Flags

Ensure the host kernel enables eBPF and raw packet socket interfaces:

```bash
# Verify active kernel flags
cat /boot/config-$(uname -r) | grep -E 'CONFIG_BPF_SYSCALL|CONFIG_NET_RAW|CONFIG_XDP_SOCKETS'
```

* `CONFIG_BPF_SYSCALL=y`: Allows userspace programs to load eBPF bytecode.
* `CONFIG_PACKET=y`: Enables `AF_PACKET` raw socket injection.
* `CONFIG_XDP_SOCKETS=y`: Enables zero-copy AF_XDP ring descriptors.
```

