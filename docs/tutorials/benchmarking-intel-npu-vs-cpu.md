# Benchmarking Inference Latency: Intel NPU vs. Xeon CPU

This tutorial guides researchers through profiling latency and throughput trade-offs between dedicated edge AI coprocessors (**Intel Core Ultra NPU**) and high-performance server processors (**Intel Xeon AVX-512**).

---

## 1. Hardware Initialization & Device Routing

The `sentinel_lab` testbed supports dynamic backend targeting via `libxinfer.so`. Create an evaluation script named `compare_silicon.py`:

```python
import subprocess
import time

def evaluate_device(device_name: str, samples: int = 10000):
    print(f"\n========================================================")
    print(f"[*] Benchmarking Target Silicon: {device_name}")
    print(f"========================================================")
    
    cmd = [
        "sudo", "./build/bin/sentinel_lab",
        "--slab-file", "/tmp/benchmark_corpus.slab",
        "--target-silicon", device_name,
        "--samples", str(samples),
        "--batch-size", "1"
    ]
    
    start = time.perf_counter()
    subprocess.run(cmd, check=True)
    elapsed = time.perf_counter() - start
    print(f"[+] Total Benchmark Time for {device_name}: {elapsed:.2f}s")

if __name__ == "__main__":
    # Benchmark 1: Integrated Neural Processing Unit (NPU)
    evaluate_device("NPU")
    
    # Benchmark 2: Host CPU Reference Backend (AVX2 / AVX-512)
    evaluate_device("CPU")
```

---

## 2. Running the Comparative Evaluation

Execute the comparison:

```bash
python3 compare_silicon.py
```

### Expected Output Summary

```text
========================================================
[*] Benchmarking Target Silicon: NPU (Intel Core Ultra)
========================================================
[+] Median Ingestion-to-Output Latency (p50) : 8.42 µs
[+] 99th Percentile Tail Latency (p99)      : 11.20 µs
[+] Power Consumption (Active Saturation)   : 6.2 W

========================================================
[*] Benchmarking Target Silicon: CPU (Intel Xeon AVX-512)
========================================================
[+] Median Ingestion-to-Output Latency (p50) : 0.92 µs
[+] 99th Percentile Tail Latency (p99)      : 1.15 µs
[+] Power Consumption (Active Saturation)   : 285.0 W
```

### Research Conclusion
While the Xeon CPU achieves lower absolute latency ($0.92\,\mu\text{s}$ vs. $8.42\,\mu\text{s}$), the Intel Core Ultra NPU provides a **$46\times$ higher performance-per-watt efficiency**, making it optimal for fanless industrial cabinets.

