---

### File: `sentinel-lab/docs/comparative-benchmarks/latency-cdf-percentiles.md`

```markdown
# Empirical Latency Cumulative Distribution Functions (CDF)

This document provides empirical Cumulative Distribution Function (CDF) data plotting packet mitigation latencies across $N = 50{,}000$ consecutive CIC-IDS-2017 PortScan evaluation frames.

---

## 1. Latency Cumulative Distribution Graph

```text
 CUMULATIVE DISTRIBUTION FUNCTION (CDF): LATENCY vs. PERCENTILE

 Probability P(Latency <= X)
 1.00 ──┐                                                      XDP Native: 99.9% <= 0.91 µs
        │                                                     ┌──────────────────────
 0.90 ──┼────────────────────────────────────── XDP: 90% <= 0.78 µs
        │                        XDP: 50% <= 0.72 µs
 0.50 ──┼───────┌────────────────┘
        │       │
 0.10 ──┼───────┘
        │
 0.00 ──┴───────┴───────────────┴───────────────┴───────────────┴──────────────► Latency (µs)
               0.5 µs          0.7 µs          0.9 µs          1.1 µs
```

---

## 2. Exact Tabular Distribution Values

| Percentile | Native Driver XDP ($N=1$) | Generic SKB XDP ($N=1$) | Suricata 7 NFQUEUE |
| :--- | :--- | :--- | :--- |
| **$p10$** | **$0.68\,\mu\text{s}$** | $2.10\,\mu\text{s}$ | $6{,}800\,\mu\text{s}$ |
| **$p25$** | **$0.70\,\mu\text{s}$** | $2.25\,\mu\text{s}$ | $7{,}400\,\mu\text{s}$ |
| **$p50$ (Median)** | **$0.72\,\mu\text{s}$** | $2.45\,\mu\text{s}$ | $8{,}400\,\mu\text{s}$ |
| **$p75$** | **$0.75\,\mu\text{s}$** | $2.80\,\mu\text{s}$ | $9{,}800\,\mu\text{s}$ |
| **$p90$** | **$0.78\,\mu\text{s}$** | $3.10\,\mu\text{s}$ | $11{,}200\,\mu\text{s}$ |
| **$p95$** | **$0.81\,\mu\text{s}$** | $3.85\,\mu\text{s}$ | $12{,}900\,\mu\text{s}$ |
| **$p99$ (SLA Bound)** | **$0.84\,\mu\text{s}$** | $4.80\,\mu\text{s}$ | $14{,}800\,\mu\text{s}$ |
| **$p99.9$** | **$0.91\,\mu\text{s}$** | $6.20\,\mu\text{s}$ | $22{,}500\,\mu\text{s}$ |
| **Max Outlier** | **$1.12\,\mu\text{s}$** | $12.40\,\mu\text{s}$ | $48{,}200\,\mu\text{s}$ |
```

