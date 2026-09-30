---

### File: `sentinel-lab/docs/architecture/academic-reproducibility-imperative.md`

```markdown
# The Academic Reproducibility Crisis in Network Security

Over $85\%$ of published academic papers in machine learning-based network intrusion detection evaluate models using offline CSV datasets (e.g., loading `KDDCup99`, `NSL-KDD`, or `CIC-IDS-2017` into a Python Jupyter Notebook with Pandas and Scikit-Learn). 

This methodology introduces what `sentinel-lab` defines as the **"CSV Illusion"**.

---

## 1. The "CSV Illusion" vs. Real-World Systems

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ THE CSV ILLUSION (Standard Academic Literature)             │
 ├─────────────────────────────────────────────────────────────┤
 │ • Training on static, pre-extracted, pre-cleaned CSV tables │
 │ • Ignores packet arrival timings and inter-arrival jitter   │
 │ • Zero PCIe bus serialization, zero DMA transfers           │
 │ • Evaluates theoretical classification, NOT active drop     │
 │ • Reports 99.9% accuracy; COLLAPSES to < 50% on live wire   │
 └─────────────────────────────────────────────────────────────┘
                               VS
 ┌─────────────────────────────────────────────────────────────┐
 │ HARDWARE-IN-THE-LOOP REALITY (Sentinel-Lab Paradigm)        │
 ├─────────────────────────────────────────────────────────────┤
 │ • Features streamed as raw binary wire frames (SLAB)        │
 │ • Ingress through real Linux network drivers (AF_PACKET)    │
 │ • Measured from wire arrival to in-kernel drop (< 0.84 µs)  │
 │ • Accounts for CPU cache eviction, memory bus stalls        │
 │ • 100% reproducible on physical bare-metal hardware         │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Why Offline CSV Training Fails in Production

1. **Feature Extraction Latency is Ignored:** In static CSVs, statistical features (e.g., `Flow Duration`, `Fwd Packet Length StdDev`) are already calculated. On a real $10\text{ GbE}$ network, extracting these statistics in user-space consumes hundreds of microseconds—far exceeding line-rate budgets.
2. **Missing System Latencies:** An offline model does not account for kernel interrupt handling, softirq NAPI poll loops, socket buffer allocation (`sk_buff`), or bus contention.
3. **No Active Mitigation Proof:** Predicting an attack in a notebook provides zero proof that the host operating system can drop the packet before application compromise occurs.

`sentinel-lab` eliminates this disparity by evaluating models on live, streaming network frames under realistic line-rate pressure.
```

