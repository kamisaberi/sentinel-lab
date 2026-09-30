# Verifying Environment

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Checking raw socket capabilities, eBPF JIT, and memory limits.

## Sockets

CAP_NET_RAW or root; AF_PACKET rings need locked-memory headroom.

## JIT & memory

JIT enabled, BTF present, mlock limits sized for UMEM arenas.

```bash
$ sysctl net.core.bpf_jit_enable
$ ls /sys/kernel/btf/vmlinux
$ ulimit -l   # locked memory for rings
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
