# Cross-Silicon Normalization Standards & Benchmarking Rules

To ensure academic validity under peer-review standards, `sentinel-lab` enforces six benchmarking rules across all silicon evaluations.

---

## 1. The Six Normalization Rules

| Rule Number | Mandate | Technical Enforcement |
| :--- | :--- | :--- |
| **Rule 1** | **Batch Size Invariant ($N=1$)** | All latency percentiles must be recorded with batch size $1$. Large batches ($N \ge 64$) are prohibited for fast-path claims. |
| **Rule 2** | **Precision Normalization** | Models must be evaluated under matched precisions: **INT8** (Quantized Post-Training) or **FP16** (Half Precision). |
| **Rule 3** | **Memory Boundary Transparency** | Timing must encompass host-to-device bus transfer, kernel execution, and device-to-host readback. |
| **Rule 4** | **CPU Core Shielding** | Execution threads must be pinned to isolated CPU cores excluded from OS scheduling via `isolcpus`. |
| **Rule 5** | **Cache Pre-Warming** | Runtimes must execute a minimum of $10{,}000$ warm-up inferences before recording data. |
| **Rule 6** | **Sample Size Uniformity** | All cumulative distribution functions (CDF) must record a minimum of $N = 50{,}000$ continuous evaluations. |

---

## 2. Eliminating Python Jitter

In many academic papers, Python garbage collection or global interpreter lock (GIL) contention introduces latency spikes of $500 - 2000\,\mu\text{s}$. 

`sentinel-lab` benchmarks run entirely in **compiled ISO C++20**, eliminating runtime garbage collection pauses and ensuring measurement reproducibility.

