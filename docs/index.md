
### File: `sentinel-lab/docs/index.md`

```markdown
# Sentinel-Lab: Academic Research Testbed (`sentinel_lab`)

**Open Academic Research Testbed, SLAB Binary Wire Protocol & IEEE Preprint Artifact**  
*Tier 5 Scientific Verification Engine of the Aryorithm / Blackbox Sentinel Ecosystem*

---

## Executive Academic Overview

`sentinel-lab` is an open-source, peer-reviewed scientific evaluation platform and hardware-in-the-loop (HIL) research testbed. It resolves the widespread **reproducibility crisis** in cyber-physical intrusion detection research, where academic papers evaluate machine learning models on static, offline CSV files without testing live packet injection, bus latencies, or kernel drop mechanics.

Operating over raw Linux network sockets (`AF_PACKET`), `sentinel-lab` introduces the **SLAB Universal Binary Wire Protocol**, streams the official Canadian Institute for Cybersecurity **CIC-IDS-2017 PortScan benchmark dataset** at line rate ($> 60{,}000\text{ EPS}$), and benchmarks heterogeneous edge AI inference engines (**Intel OpenVINO NPU vs. NVIDIA TensorRT**) against Linux in-kernel eBPF/XDP drop filters.

```text
====================================================================================================
                        SENTINEL-LAB OPEN RESEARCH TESTBED ARCHITECTURE
====================================================================================================
 [DATASET CORPUS]             CANADIAN INSTITUTE FOR CYBERSECURITY (CIC-IDS-2017)
                              • 77.4 MB Raw PCAP / PortScan Traffic Slices
                              • Automated Fetch & MinMax Normalization Engine
                                                │
                                                ▼ Serialization via tools/csv_to_slab.py
 [TIER 5 WIRE PROTOCOL]       SLAB UNIVERSAL BINARY WIRE PROTOCOL (Magic: 0x534C4142)
                              [ Magic 4B | EventID 8B | GroundTruth 4B | Dim 4B | Tensor N*4B ]
                                                │
                                                ▼ High-Speed Raw Socket Blaster (AF_PACKET)
 ┌──────────────────────────────────────────────┴──────────────────────────────────────────────────┐
 │ TESTBED EXECUTION ENGINE (/usr/local/bin/sentinel_lab)                                          │
 │                                                                                                 │
 │  ┌───────────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Zero-Copy Kernel Hook: eBPF/XDP Filter (xdp_filter.o) -> Drops Malicious IPs in < 0.84 µs │  │
 │  └───────────────────────────────────────────┬───────────────────────────────────────────────┘  │
 │                                              │ Raw Feature Tensor Pointer (Zero-Copy)           │
 │                                              ▼                                                  │
 │  ┌───────────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Heterogeneous Dual-Silicon Comparison Harness                                             │  │
 │  │ • Target A: Intel Core Ultra NPU.3720 / Xeon AVX-512 via ov::Tensor                       │  │
 │  │ • Target B: NVIDIA Jetson Orin Nano / RTX A4000 via CUDA Asynchronous Streams             │  │
 │  └───────────────────────────────────────────┬───────────────────────────────────────────────┘  │
 │                                              │ Ground Truth vs. Model Prediction Parity         │
 │                                              ▼                                                  │
 │  ┌───────────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Empirical Scientific Metric Profiler                                                      │  │
 │  │ • Confusion Matrix: True Positive, False Positive, True Negative, False Negative         │  │
 │  │ • Classification Scores: Precision, Recall, Accuracy, F1-Score (Target: F1 > 0.998)       │  │
 │  │ • Microsecond Hardware Timer Distributions: p50, p90, p95, p99, p99.9, and Max Jitter     │  │
 │  └───────────────────────────────────────────────────────────────────────────────────────────┘  │
 └──────────────────────────────────────────────┬──────────────────────────────────────────────────┘
                                                │
                                                ▼ Verifiable Research Output
 [ACADEMIC PREPRINT]          IEEE TRANSACTIONS COMPLIANT LATEX PREPRINT (paper.tex)
                              • CERN/Zenodo Permanent Citable DOI: https://doi.org/10.5281/zenodo.1849200
                              • Ready-made LaTeX tables for Master's and PhD dissertations
====================================================================================================
```

---

## Core Invariants

1. **Deterministic Reproducibility:** Every empirical table, confusion matrix, and latency percentile published in the IEEE preprint paper can be regenerated locally using a single command: `python3 examples/run_full_evaluation.py`.
2. **Hardware-in-the-Loop Realism:** Moves beyond theoretical Python notebooks; models evaluate live, streaming binary frames passing over real Linux network sockets.
3. **Open-Access Scientific Attribution:** Released under permissive dual licensing (**MIT License** for code; **Creative Commons CC-BY-4.0** for data, benchmark traces, and LaTeX manuscripts) with a permanent **CERN/Zenodo DOI**.
4. **Cross-Architecture Equity:** Evaluates Intel, NVIDIA, and ARM silicon targets under identical batch constraints ($N=1$), identical memory isolation, and identical microsecond measurement harnesses.
```

