# Sentinel-Lab: Academic Research Kit (ARK)
### Open CPS Security Benchmarking, Reproducibility & Educational Testbed

**License:** Apache License 2.0  
**Permanent DOI:** [10.5281/zenodo.10892341](https://doi.org/10.5281/zenodo.10892341)  
**Corporate Research Lab:** Aryorithm Technologies B.V. (Amsterdam, The Netherlands)  
**Lead Architect:** Kamran Saberifard (`kamisaberi`)

---

## 1. Executive Summary for Principal Investigators (PIs) & Faculty

Evaluating cyber-physical systems (CPS) active defense mechanisms has historically suffered from the **reproducibility barrier**: algorithms evaluated solely in static Python notebooks or offline PCAP replays often fail to sustain line-rate execution without inducing physical plant instability.

**Sentinel-Lab** provides universities and research groups with a fully reproducible, zero-cost, open-source experimental instrument providing:
* **Sub-Microsecond Physical Verification:** In-kernel eBPF driver drops ($< 0.84\,\mu\text{s}$) via AF_XDP Zero-Copy UMEM.
* **Closed-Loop Physical Coupling:** 4th-Order Runge-Kutta (RK4) numerical ODE continuous physics integration with Control Barrier Function (CBF) $h(x) \ge 0$ safety assertions.
* **14+ Scientific Modules:** Extreme Value Theory (EVT) WCET bounds, microarchitectural PMU cache-miss profiling, cross-modal causal deception, continual learning drift ($\text{BWT}/\text{FWT}$), and 3GPP 5G GTP-U slice guards.
* **Zero Cloud Dependency:** 100% on-premises execution with $0.00 cloud egress.

---

## 2. Quickstart for Doctoral Researchers & Students

### Step 1: Launch the Evaluation Daemon
```bash
aryo lab daemon --port 9090