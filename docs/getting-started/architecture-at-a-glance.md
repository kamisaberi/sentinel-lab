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

