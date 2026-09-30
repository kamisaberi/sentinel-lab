---

### File: `sentinel-lab/docs/slab-protocol/protocol-validation-checks.md`

```markdown
# Protocol Validation Checks & Fuzzing Resilience

When streaming binary packets over raw network sockets, the parser must handle truncated frames, malformed headers, and network noise without throwing uncaught exceptions or crashing the research daemon.

---

## 1. Automated Validation Checklist

```text
 Incoming Buffer: packet_buffer (Size: S bytes)
                      │
                      ▼ Check 1: Minimum Size
 [ S >= 24 Bytes (sizeof(SlabHeader))? ] ──── NO ──► Drop Packet (Truncated)
                      │ YES
                      ▼ Check 2: Magic Synchronization Token
 [ header->magic == 0x534C4142? ] ─────────── NO ──► Drop Packet (Invalid Protocol)
                      │ YES
                      ▼ Check 3: Dimension Boundary Sanity
 [ header->dimensions <= 1024? ] ──────────── NO ──► Drop Packet (Dimension Overflow)
                      │ YES
                      ▼ Check 4: Payload Completeness
 [ S >= 24 + (dimensions * 4)? ] ──────────── NO ──► Drop Packet (Incomplete Payload)
                      │ YES
                      ▼
 [ PROCEED TO ZERO-COPY INFERENCE EVALUATION ]
```

---

## 2. In-Kernel eBPF Filter Rejection

Before frames reach the user-space research harness, the in-kernel eBPF program (`xdp_filter.o`) performs an initial bounds check:

```c
// bpf/xdp_filter.c
if ((void *)(slab_hdr + 1) > data_end) {
    return XDP_PASS; // Frame too small to contain a SLAB header
}
```

This prevents user-space memory corruption even under malicious packet flooding.
```

