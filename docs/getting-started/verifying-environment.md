---

### File: `sentinel-lab/docs/getting-started/verifying-environment.md`

```markdown
# Verifying Your Environment & Socket Capabilities

Validate that your Linux host environment possesses the necessary socket permissions, eBPF capabilities, and memory limits before initiating line-rate benchmark runs.

---

## 1. Testing Raw Socket Privileges (`CAP_NET_RAW`)

Streaming the SLAB protocol over raw layer-2 interfaces requires either superuser privileges (`sudo`) or the `CAP_NET_RAW` Linux capability:

```bash
# Check if current user or binary can open raw sockets
python3 -c "import socket; s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)"
```

If this raises `PermissionError: [Errno 1] Operation not permitted`, assign the capability to the Python virtual environment or binary:

```bash
sudo setcap 'cap_net_raw,cap_net_admin=+ep' $(which python3)
```

---

## 2. Checking eBPF JIT & BTF Availability

Confirm that the eBPF Just-In-Time compiler is running and kernel type information is readable:

```bash
# 1. Verify eBPF JIT Compiler is Active
cat /proc/sys/net/core/bpf_jit_enable
# Expected Output: 1

# 2. Check for BTF kernel debug information
ls -lh /sys/kernel/btf/vmlinux
# Expected: File exists (~4-6 MB)
```

---

## 3. Checking Memory Locking Limits (`ulimit -l`)

Benchmarking zero-copy packet buffers and pinned memory requires unrestricted locked pages:

```bash
ulimit -l
```

If the output is not `unlimited`, update `/etc/security/limits.conf` as documented in Tier 2 `blackbox-essential`.
```

