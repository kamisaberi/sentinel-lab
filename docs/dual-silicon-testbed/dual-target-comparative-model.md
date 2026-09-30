### Part 4: Heterogeneous Silicon Comparison (`dual-silicon-testbed/*`)

This section contains 5 technical benchmark specifications and comparative research architectures for `sentinel-lab`: the dual-target comparative methodology, the Intel OpenVINO NPU pipeline, the NVIDIA TensorRT CUDA stream pipeline, cross-silicon normalization standards, and thermal/power consumption profiling.

---

### File: `sentinel-lab/docs/dual-silicon-testbed/dual-target-comparative-model.md`

```markdown
# Dual-Target Comparative Methodology: 1-to-1 Stream Parity

In academic literature, comparing artificial intelligence accelerators (such as comparing an Intel NPU against an NVIDIA GPU) is often biased by divergent benchmark harnesses: evaluating one framework in Python while profiling another in native C++, or running batch sizes of $N=64$ on GPU while restricting CPU tests to $N=1$.

`sentinel-lab` eliminates benchmarking bias through **1-to-1 Stream Parity**.

---

## 1. Experimental Parity Architecture

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ REPRODUCIBLE TRAFFIC SOURCE: 10GbE SLAB STREAM (N=1)        │
 │  - Replays 50,000 packets of CIC-IDS-2017 PortScan          │
 │  - Transmitted over physical fiber to both targets          │
 └──────────────────────────────┬──────────────────────────────┘
                                │
        ┌───────────────────────┴───────────────────────┐
        ▼ Target Silicon A                              ▼ Target Silicon B
 ┌─────────────────────────────┐         ┌─────────────────────────────┐
 │ Intel Core Ultra 7 165H     │         │ NVIDIA Jetson AGX Orin      │
 │  - Intel Level Zero Driver  │         │  - CUDA 12.2 Driver         │
 │  - OpenVINO 2024.1 Runtime  │         │  - TensorRT 10.0 Runtime    │
 │  - Backend: ov::Tensor wrap │         │  - Backend: cudaStream_t    │
 └──────────────┬──────────────┘         └──────────────┬──────────────┘
                │                                       │
                ▼ Identical Metrics Evaluated           ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. Fast-Path Latency (Nanosecond TSC Cycles: p50 to p99.9)  │
 │ 2. Classification Accuracy, Precision, Recall, and F1       │
 │ 3. Physical Power Dissipation (Joules per Classification)   │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Controlled Invariants

1. **Identical Mathematical Weights:** Both silicon engines execute identical trained weights (`network_threat_v2.onnx`), quantized using standardized symmetric INT8 calibration.
2. **Identical Memory Boundaries:** Both harnesses measure time starting from the moment packet bytes enter the raw socket buffer until the prediction score is written to RAM.
3. **Single-Frame Fast Path ($N=1$):** Benchmarks strictly evaluate batch size $N=1$ to reflect real-world, inline wire-speed packet mitigation constraints.
```

