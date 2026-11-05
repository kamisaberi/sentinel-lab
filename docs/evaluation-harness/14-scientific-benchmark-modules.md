# The 14 Scientific Research Benchmark Modules in `sentinel-lab`

`sentinel-lab` includes **14 specialized C++20 research modules** located in `src/modules/`. Each module provides standardized, reproducible scientific evaluation of a specific aspect of edge cyber-physical security.

---

## 1. Scientific Module Directory & Research Objectives

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ SENTINEL-LAB 14 SCIENTIFIC RESEARCH MODULES                 │
 ├─────────────────────────────────────────────────────────────┤
 │ [01] hil_coupler          : 4th-Order Runge-Kutta (RK4) ODE │
 │ [02] adversarial_per...  : Bounded L_inf / L_2 Perturbation│
 │ [03] microarch_profiler  : Linux perf_event_open PMU IPC   │
 │ [04] scada_smuggler      : Differential Protocol Desync    │
 │ [05] fdia_suite          : Cross-Modal False Data Injection│
 │ [06] continual_drift     : BWT & FWT Task Matrix Drift     │
 │ [07] wcet_engine         : Extreme Value Theory (EVT) Gumbel│
 │ [08] quant_drift         : Multi-Silicon MRD Attribution   │
 │ [09] pqc_overhead        : NIST ML-KEM/ML-DSA TLS 1.3 MTU  │
 │ [10] barrier_verifier    : Nagumo Control Barrier Functions│
 │ [11] stateful_fuzzer     : Markov Session State Injector   │
 │ [12] em_telemetry        : CPU Power & Side-Channel Wave   │
 │ [13] byzantine_eval      : Krum & Trimmed-Mean Poisoning   │
 │ [14] patch_regression    : Pre- vs Post-Patch .spkg Differential│
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Detailed Module Specifications

### Module 1: `hil_coupler` (Physical Plant HIL Solver)
Executes a 4th-Order Runge-Kutta (RK4) continuous ODE solver modeling physical generator rotor angle stability:
$$\frac{d\delta}{dt} = \omega - \omega_0, \quad \frac{d\omega}{dt} = \frac{1}{M} \left(P_m - P_e - D(\omega - \omega_0)\right)$$
* **Evaluated SLA:** Proves that in-kernel drops ($< 0.84\,\mu\text{s}$) restore plant equilibrium before the Critical Clearing Time ($T_{\text{crit}} = 38.4\text{ ms}$) is exceeded.

### Module 2: `adversarial_perturbator`
Generates grammar-constrained bounded $\epsilon$-perturbations ($L_\infty \le 0.05$) across network feature vectors to measure adversarial robustness boundaries.

### Module 3: `microarch_profiler`
Interfaces with Linux `perf_event_open` hardware PMU counters, measuring Instructions Per Cycle (**2.42 IPC**) and L3 Last Level Cache miss rates (**1.24%**).

### Module 7: `wcet_engine` (EVT Latency Bound)
Fits Block Maxima latency samples to a Generalized Extreme Value (GEV) Gumbel distribution, mathematically proving the $p99.999\%$ Worst-Case Execution Time ($1.82\,\mu\text{s}$).

### Module 9: `pqc_overhead`
Profiles NIST Post-Quantum Cryptography (ML-KEM-768 & ML-DSA-65) key exchange fragmentation and TLS 1.3 packet expansion overhead.

### Module 10: `barrier_verifier`
Evaluates Nagumo forward-invariance conditions on Control Barrier Functions (CBF):
$$\dot{h}(x, u) + \gamma(h(x)) \ge 0$$
Proves mathematically that field actuators cannot enter unsafe physical states.
