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

