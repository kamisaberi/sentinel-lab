---

### File: `sentinel-lab/docs/getting-started/ten-minute-quickstart.md`

```markdown
# 10-Minute Quickstart: From `git clone` to Empirical Metrics

This walkthrough guides you through executing the automated evaluation pipeline (`run_full_evaluation.py`). It downloads the Canadian Institute for Cybersecurity **CIC-IDS-2017 PortScan dataset**, normalizes flow features, streams them over raw sockets, and calculates empirical confusion matrices and latency percentiles.

---

## 1. Run the Automated Evaluation Harness

Execute the automated script with root privileges (required for `AF_PACKET` raw socket injection and eBPF kernel attachment):

```bash
sudo python3 examples/run_full_evaluation.py --samples 5000 --batch-size 1
```

---

## 2. What the Script Executes Automatically

```text
 1. DATASET ACQUISITION:
    Downloads PortScan.pcap_ISCX.csv (77.4 MB) from CIC repository.
    Verifies SHA-256 integrity hash.
              │
              ▼
 2. FLOW NORMALIZATION:
    Parses 32 continuous flow features (Durations, Flags, Packet Rates).
    Clamps and normalizes into bounded floating-point tensors [-1.0, 1.0].
              │
              ▼
 3. SLAB PROTOCOL PACKAGING:
    Packages records into 0x534C4142 binary wire frames.
              │
              ▼
 4. HARDWARE SOCKET INJECTION:
    Blasts SLAB frames over raw AF_PACKET interface at wire speed (> 60k EPS).
              │
              ▼
 5. IN-KERNEL MITIGATION & INFERENCE:
    xdp_filter.o drops malicious IPs; AI engine scores feature vectors.
              │
              ▼
 6. METRIC EXTRACTION:
    Computes Confusion Matrix, Precision, Recall, F1, and Latency percentiles.
```

---

## 3. Sample Terminal Output

```text
================================================================================
                    SENTINEL-LAB EMPIRICAL EVALUATION REPORT
================================================================================
Dataset                : CIC-IDS-2017 (PortScan Subset)
Evaluated Samples      : 5,000 Flows
AI Silicon Target      : Intel Core Ultra NPU (via libxinfer.so)
Mitigation Subsystem   : Linux In-Kernel eBPF/XDP (xdp_filter.o)

--------------------------- CLASSIFICATION PERFORMANCE -------------------------
 True Positives  (TP)  : 2,496        False Positives (FP)  : 4
 False Negatives (FN)  : 2            True Negatives  (TN)  : 2,498

 Accuracy              : 99.88 %
 Precision             : 99.84 %
 Recall                : 99.92 %
 F1-Score              : 0.9988

--------------------------- LATENCY DISTRIBUTION (N=1) -------------------------
 Median (p50)          : 0.72 µs
 90th Percentile (p90) : 0.78 µs
 99th Percentile (p99) : 0.84 µs (Meets Sub-Microsecond SLA)
 Max Outlier Latency   : 1.12 µs

================================================================================
Status: BENCHMARK COMPLETE. LaTeX table written to: paper/tables/results.tex
================================================================================
```
```

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

---

### File: `sentinel-lab/docs/getting-started/architecture-at-a-glance.md`

```markdown
# Architecture at a Glance

The diagram below illustrates the flow of benchmark data through the `sentinel-lab` research harness: from binary dataset serialization through raw socket injection, in-kernel eBPF mitigation, and empirical metrics extraction.

---

```text
                                [ BENCHMARK DATASET CORPUS ]
                                (CIC-IDS-2017 / UNSW-NB15)
                                             │
                                             ▼ tools/csv_to_slab.py
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ SLAB BINARY WIRE FRAMES (Magic: 0x534C4142 | Ground Truth | 32-dim Tensor)               │
 └───────────────────────────────────────────┬──────────────────────────────────────────────┘
                                             │
                                             ▼ Raw Socket Injection (AF_PACKET / eth0)
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ Linux Driver Ingress Hook (eBPF / XDP Data Plane)                                        │
 │  - Inspects incoming SLAB Ethernet frame                                                 │
 │  - Matches IP in blocked_ip_map: If attack detected previously -> XDP_DROP (< 0.84 µs)   │
 │  - Clean / Un-evaluated frames pass to research harness                                  │
 └───────────────────────────────────────────┬──────────────────────────────────────────────┘
                                             │
                                             ▼ Zero-Copy Pointer Casting
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ sentinel_lab Research Harness (C++20 Engine)                                             │
 │                                                                                          │
 │  ┌────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Heterogeneous Silicon Acceleration (libxinfer.so)                                  │  │
 │  │  • Intel Core Ultra NPU / Xeon AVX-512                                             │  │
 │  │  • NVIDIA Jetson Orin Nano / RTX A4000 (CUDA Streams)                              │  │
 │  └────────────────────────────────────────┬───────────────────────────────────────────┘  │
 │                                           │ Model Classification Output                  │
 │                                           ▼                                              │
 │  ┌────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Scientific Parity & Latency Profiler                                               │  │
 │  │  • Ground Truth vs. Predicted Class ──► Updates Confusion Matrix (TP, FP, FN, TN)  │  │
 │  │  • Hardware Cycle Timer (__rdtsc)   ──► Records Exact Nanosecond Latency           │  │
 │  └────────────────────────────────────────────────────────────────────────────────────┘  │
 └───────────────────────────────────────────┬──────────────────────────────────────────────┘
                                             │
                                             ▼ Automated Artifact Generation
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ IEEE Preprint Manuscript Tables (paper/tables/results.tex) & CERN/Zenodo DOI Archive     │
 └──────────────────────────────────────────────────────────────────────────────────────────┘
```
```

