# `sentinel-lab` — Cyber-Physical Research Testbed & Scientific Evaluation Engine

[![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Python 3.11+](https://img.shields.io/badge/Python-3.11%2B-green.svg)](https://www.python.org/)
[![License](https://img.shields.io/badge/License-Proprietary%20%2F%20Academic-orange.svg)](#license)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.10892341.svg)](https://doi.org/10.5281/zenodo.10892341)
[![Ecosystem](https://img.shields.io/badge/Ecosystem-Aryorithm%20Sentinel-cyan.svg)](https://aryorithm.com)

**`sentinel-lab`** is the Tier 5 scientific research testbed and experimental validation engine of the **Aryorithm Blackbox Sentinel** active defense platform. It transforms cybersecurity research from slow, static CSV/PCAP offline analysis into an **air-gapped, hardware-aware, closed-loop cyber-physical evaluation suite**.

Designed for systems researchers, defense engineers, and university labs, `sentinel-lab` provides a 14-dimension scientific research suite capable of evaluating real-time worst-case execution time (WCET) determinism, CPU microarchitecture cache miss rates, closed-loop physical plant differential equations, formal control barrier functions, SCADA request smuggling zero-days, post-quantum cryptography (PQC) overheads, and multi-silicon quantization drift—**without requiring million-euro physical SCADA testbed hardware**.

---

## 1. Aryorithm 8-Tier Architecture Context

```text
========================================================================================================
                                     ARYORITHM 8-TIER ARCHITECTURE
========================================================================================================

 [TIER 6: COMMAND PLANE]       SENTINEL-NEXUS: Fleet Orchestrator • Sub-50ms Collective Defense Grid
                                • gRPC 50051 • 1440px Boxed Web C2 9443 • Real-Time SSE Stream 9444
                                                │
         ┌──────────────────────────────────────┴──────────────────────────────────────┐
         ▼                                                                             ▼
 [TIER 4: CONTINUAL AI]                                                [TIER 5: RESEARCH TESTBED]
  xInfer-Forge (forge-cli)                                              Sentinel-Lab (sentinel_lab)  <-- THIS REPO
  • Tabular MAE + InfoNCE Retraining                                    • SLAB Binary Protocol (0x534C4142)
  • Immutable Golden Attack Safety Gate                                 • CERN/Zenodo DOI Preprint
         │                                                                             │
         └──────────────────────────────────────┬──────────────────────────────────────┘
                                                │
 [TIER 3: EDGE XDR APPLIANCE]  BLACKBOX-SENTINEL (sentinel daemon)
                               • 26 Decoupled Native Subsystems • 30 Industrial Protocol Plugins
                               • 4-Stage ModelRouter Pipeline (SPKG Reflex -> Dissectors -> AI -> Fusion)
                                                │
 [TIER 2: MITIGATION CORE ]    BLACKBOX-ESSENTIAL (libblackbox.so)
                               • In-Kernel eBPF CO-RE Filter (xdp_drop.o via vmlinux.h & BTF Relocations)
                               • True AF_XDP Zero-Copy UMEM 4-Ring Engine (Adaptive Dual-Mode)
                                                │
 [TIER 1: SILICON RUNTIME ]    XINFER-ESSENTIAL (libxinfer.so)
                               • Universal Zero-Copy Memory Pointers (15 Silicon Target Backends)
========================================================================================================
 [TIER 7: CYBER-RANGE MESH]    SENTINEL-MATRIX (VMware & Docker 10.240.0.0/24 Multi-Container Digital Twin)
 [TIER 8: META-INSTALLER]      SENTINEL-STACK (1-Click Topological DAG Compiler)
 [PACKAGE REGISTRY HUB]        SENTINEL-HUB (github.com/kamisaberi/sentinel-hub)
========================================================================================================
```

---

## 2. The 14 Scientific Research Capabilities

`sentinel-lab` integrates **14 specialized, hardware-aware research modules**:

```text
========================================================================================================================
                                     SENTINEL-LAB 14-FEATURE RESEARCH SUITE
========================================================================================================================
 Feature #  Feature Name                         Core Technical / Scientific Focus                      Key Scientific Metric
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 [1] Closed-Loop Physical Co-Simulation  4th-Order Runge-Kutta (RK4) ODE solver            Critical Clearing Time (T_crit),
     (HIL / ODE Coupler)                  coupled to SCADA network delay jitter             Lyapunov Exponent
 [2] Grammar-Constrained Adversarial     Protocol-compliant bounded ε-perturbation          Evasion Rate vs.
     Perturbator                          balls (L_inf / L_2) on feature manifolds          Semantic Validity
 [3] Microarchitectural Hardware PMU     Linux perf_event_open CPU hardware event          Instructions/Cycle (IPC),
     Profiler                             counters for LLC cache misses & bus locks          LLC Miss Rate (%)
 [4] Differential Parser Desync Fuzzer   SCADA Request Smuggling oracle comparing          Parser Desync Vectors
     (SCADA Bug Hunter)                   gateway verdict vs. target PLC state change       (V_gateway != V_plc)
 [5] Cross-Modal Causal FDIA Suite       Synchronized dual-stream (spoofed cyber vs.       Time-to-Divergence (T_det),
     (Stuxnet / TRITON Deception)         true physical drift) testing ThreatCorrelator     Causal F1-Score
 [6] Continual Learning Drift Profiler   Task Matrix R_ij evaluating catastrophic          Backward Transfer (BWT),
     (Catastrophic Forgetting)            forgetting across sequential threat epochs        Forward Transfer (FWT)
 [7] Extreme Value WCET Determinism      Block maxima sampling & Gumbel distribution       p99.999% Statistical
     Engine                               fitting via Method of Moments                     WCET Upper Bound
 [8] Multi-Silicon Precision Drift       Cosine similarity & Spearman rank correlation     Cosine Similarity, MRD XAI
     Profiler                             on MRD XAI attributions across 15 targets         Rank Inversion Rate (%)
 [9] Post-Quantum Cryptography (PQC)     NIST ML-KEM / ML-DSA TLS 1.3 key size             IP Fragment Count,
     OT Overhead Profiler                 overhead & MTU fragmentation latency penalties     OT SLA Breach Status
[10] Formal Barrier Function (CBF) &      Control Barrier Functions h(x) >= 0 and           Forward-Invariance Margin,
     Reachable Set Verifier               Nagumo condition (dh/dt + alpha*h >= 0)           Reachable Set Boundary
[11] Generative Stateful SCADA Fuzzer     Markov state machine session generator            DPI State Machine
     (Stateful Session Injector)          (COTP CR -> S7 Setup -> Read -> CPU STOP)          Tracking Evasion
[12] Hardware Side-Channel Power/EM       Pearson correlation (rho) between packet          Pearson Correlation (rho),
     Telemetry Correlator                 rates and CPU power/EM radiation spikes           Power Spike Delta (mW)
[13] Federated AI & Byzantine            Model aggregation, gradient sign-flipping          Poisoning Attack Success
     Poisoning Evaluator                  attacks & Krum / Trimmed-Mean defense             Rate (PASR %)
[14] Automated Exploit Neutralization     Pre-patch vs. post-patch differential             Neutralization Rate (%),
     & Patch Regression Engine            scorecard for .spkg reflex rules                  Throughput Delta (ΔEPS)
========================================================================================================================
```

---

## 3. Binary Wire Format: The SLAB Protocol (`0x534C4142`)

To achieve wire-speed transmission ($> 60{,}000\text{ EPS}$) over Linux raw sockets (`AF_PACKET` / `SOCK_RAW`) without serialization overhead, `sentinel-lab` utilizes the **SLAB Binary Wire Protocol**.

### Packed 24-Byte Header Specification
```cpp
#pragma once
#include <cstdint>

namespace sentinel::lab {

inline constexpr uint32_t SLAB_MAGIC = 0x534C4142; // ASCII: 'S', 'L', 'A', 'B'

#pragma pack(push, 1)
struct SlabHeader {
    uint32_t magic;          // Byte 0-3  : 0x534C4142
    uint64_t event_id;       // Byte 4-11 : Monotonic sequence identifier
    uint32_t ground_truth;   // Byte 12-15: 0 = Benign, 1 = Attack
    uint32_t dimensions;     // Byte 16-19: Feature count D (1 .. 1024)
    uint32_t flags;          // Byte 20-23: Bitmask flags / Reserved
};
#pragma pack(pop)

static_assert(sizeof(SlabHeader) == 24, "SlabHeader must be exactly 24 bytes");

} // namespace sentinel::lab
```

* **Wire Frame Layout:** `[ 24-Byte SlabHeader ] + [ D * 4-Byte IEEE 754 Float32 Tensor Payload ]`
* **Zero-Copy Handshake:** `DimensionHandshake::extract_features()` reinterpret-casts memory buffers into zero-copy `std::span<const float>` views without heap allocation.

---

## 4. Directory Structure

```text
sentinel-lab/
├── CMakeLists.txt                     # C++20 CMake build configuration
├── configs/                           # Dynamic JSON/YAML research profiles
│   ├── benchmark_profile.json         # Core line-rate injection profile
│   ├── evt_profile.json               # Feature 7: EVT WCET profile
│   ├── pmu_profile.json               # Feature 3: CPU PMU hardware event profile
│   ├── adversarial_profile.json       # Feature 2: Grammar-constrained evasion profile
│   ├── drift_profile.json             # Feature 6: Continual learning task sequence
│   ├── hil_profile.json               # Feature 1: Closed-loop HIL ODE safety bounds
│   ├── fuzzer_profile.json            # Feature 4: SCADA request smuggling profile
│   ├── quant_profile.json             # Feature 8: Multi-silicon quantization profile
│   ├── pqc_profile.json               # Feature 9: Post-quantum cryptography profile
│   ├── stateful_fuzzer_profile.json   # Feature 11: Stateful SCADA session profile
│   ├── sidechannel_profile.json       # Feature 12: Hardware side-channel trace profile
│   ├── cbf_profile.json               # Feature 10: Control Barrier Function profile
│   ├── federated_profile.json         # Feature 13: Federated learning Byzantine profile
│   └── patch_regression_profile.json  # Feature 14: Patch neutralization scorecard profile
├── include/sentinel_lab/              # Public C++20 headers
│   ├── slab_protocol.hpp             # Packed 24-byte SLAB header definition
│   ├── dimension_handshake.hpp       # Dynamic feature vector validator (1..1024D)
│   ├── evt_engine.hpp                # Feature 7: Gumbel WCET statistical estimator
│   ├── pmu_profiler.hpp              # Feature 3: Native Linux perf_event_open profiler
│   ├── adversarial_engine.hpp         # Feature 2: Native C++ feature mutator
│   ├── drift_profiler.hpp            # Feature 6: Task Matrix R_ij & BWT/FWT calculator
│   ├── hil_coupler.hpp               # Feature 1: Closed-loop HIL physical state coupler
│   ├── differential_fuzzer.hpp       # Feature 4: SCADA Request Smuggling oracle
│   ├── fdia_engine.hpp               # Feature 5: Dual-stream FDIA causal engine
│   ├── quant_profiler.hpp            # Feature 8: Cosine & MRD XAI rank inversion
│   ├── pqc_profiler.hpp              # Feature 9: NIST PQC key overhead & MTU profiler
│   ├── stateful_fuzzer.hpp           # Feature 11: Generative stateful session fuzzer
│   ├── sidechannel_correlator.hpp    # Feature 12: Pearson correlation power/EM profiler
│   ├── cbf_verifier.hpp              # Feature 10: Control Barrier Function verifier
│   ├── federated_evaluator.hpp       # Feature 13: Byzantine-resilient FL evaluator
│   └── patch_regression_engine.hpp   # Feature 14: Differential patch scorecard engine
├── src/                               # C++20 engine implementations
│   ├── main.cpp                       # Research lab daemon entry point
│   ├── engine.cpp                     # Core inference engine with PMU hooks
│   ├── benchmarker.cpp                # Evaluation report generator with EVT
│   ├── dimension_handshake.cpp       # Dynamic zero-copy feature extractor
│   ├── evt_engine.cpp                 # Method of Moments Gumbel solver
│   ├── pmu_profiler.cpp              # Linux perf_event_open counter reader
│   ├── adversarial_engine.cpp        # Line-rate C++ feature mutator
│   ├── drift_profiler.cpp            # BWT / FWT / Retention decay matrix solver
│   ├── hil_coupler.cpp               # Lyapunov exponent & T_crit calculator
│   ├── differential_fuzzer.cpp       # MBAP/COTP byte mutation & oracle engine
│   ├── fdia_engine.cpp               # Synchronized dual-stream FDIA evaluator
│   ├── quant_profiler.cpp            # Cosine & Spearman rank correlation solver
│   ├── pqc_profiler.cpp              # PQC MTU fragmentation & SLA penalty solver
│   ├── stateful_fuzzer.cpp           # S7Comm / COTP state machine generator
│   ├── sidechannel_correlator.cpp    # Pearson correlation coefficient solver
│   ├── cbf_verifier.cpp              # Nagumo forward-invariance inequality solver
│   ├── federated_evaluator.cpp       # Krum & Trimmed-Mean robust FL aggregator
│   └── patch_regression_engine.cpp   # Differential patch regression scorecard engine
├── harness/                           # Python 3.11+ research orchestrators
│   ├── socket_injector.py            # Wire-speed raw socket blaster
│   ├── adversarial_mutator.py        # Feature 2: Python grammar-constrained mutator
│   ├── drift_evaluator.py            # Feature 6: Continual learning task matrix driver
│   ├── hil_co_simulator.py           # Feature 1: Closed-loop HIL feedback orchestrator
│   ├── scada_fuzzer.py               # Feature 4: Differential gateway vs. PLC fuzzer
│   ├── fdia_orchestrator.py          # Feature 5: Stuxnet/TRITON deception orchestrator
│   ├── quant_evaluator.py            # Feature 8: Multi-silicon quantization evaluator
│   ├── pqc_tunnel_profiler.py        # Feature 9: PQC TLS 1.3 handshake profiler
│   ├── stateful_fuzzer.py           # Feature 11: Multi-step SCADA session fuzzer
│   ├── sidechannel_analyzer.py       # Feature 12: Power/EM trace analyzer
│   ├── cbf_reachability_evaluator.py # Feature 10: CBF reachability set verifier
│   ├── federated_poison_evaluator.py # Feature 13: PyTorch FL gradient poisoning harness
│   └── patch_regression_evaluator.py # Feature 14: .spkg patch regression evaluator
└── paper/                             # Publication artifacts & Zenodo LaTeX tables
    ├── figures/                       # Latency CDFs and ROC curve assets
    └── tables/                        # Pre-compiled LaTeX results (results.tex)
```

---

## 5. Prerequisites & Build Instructions

### System Dependencies
Ubuntu 22.04 LTS / 24.04 LTS / 26.04 LTS with:
* GCC 13+ or Clang 16+ (supporting C++20 features: `std::span`, `std::jthread`)
* CMake 3.20+
* `libbpf-dev`, `libelf-dev`, `libxdp-dev`
* `libxinfer.so` (Installed in `/usr/local/lib`)
* `libblackbox.so` (Installed in `/usr/local/lib`)
* Python 3.11+ (`numpy`, `torch`, `pyyaml`)

### Compilation
```bash
# 1. Clone repository
git clone https://github.com/kamisaberi/sentinel-lab.git
cd sentinel-lab

# 2. Configure CMake build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# 3. Build across all CPU cores
make -j$(nproc)

# 4. Install binary executable
sudo make install
```

---

## 6. Execution Examples & Research Harnesses

### 1. Wire-Speed Raw Socket Injection
Blasts SLAB protocol binary frames directly into NIC driver rings via Linux `AF_PACKET`:
```bash
python3 harness/socket_injector.py --interface ens160 --rate-eps 60000 --config configs/benchmark_profile.json
```

### 2. Extreme Value Theory (EVT) WCET Tail Latency Analysis
Runs high-precision clock sampling and fits Gumbel distributions ($\mu, \sigma$) to $p99.999\%$ tail latencies:
```bash
./build/sentinel_lab --evt-analysis --config configs/evt_profile.json
```

### 3. Closed-Loop Physical Co-Simulation (HIL / ODE Coupler)
Evaluates physical state trajectories $x(t)$ against $T_{crit}$ clearing time limits:
```bash
python3 harness/hil_co_simulator.py --config configs/hil_profile.json --duration 15
```

### 4. SCADA Request Smuggling Differential Fuzzing
Fuzzes Edge Gateway (`10.240.0.101`) and Mock PLC (`10.240.0.150`) simultaneously to detect parser desync zero-days:
```bash
python3 harness/scada_fuzzer.py --gateway 10.240.0.101 --plc 10.240.0.150 --port 502
```

### 5. Continual Learning Drift Evaluation (BWT & FWT Matrices)
Streams sequential task datasets ($T_1 \dots T_N$) to compile Task Matrix $R_{i,j}$ and test for catastrophic forgetting:
```bash
python3 harness/drift_evaluator.py --config configs/drift_profile.json
```

### 6. Post-Quantum Cryptography (PQC) OT Overhead Profiler
Profiles NIST ML-KEM / ML-DSA TLS 1.3 key size expansions, MTU fragmentation, and SLA latency penalties:
```bash
python3 harness/pqc_tunnel_profiler.py --config configs/pqc_profile.json
```

---

## 7. Inter-Repository Dependencies

```text
[ sentinel-hub ]     ──> Supplies certified malware PCAPs & SLAB datasets (dist/datasets/*.slab)
        │
[ sentinel-lab ]     ──> Master Research Testbed: Executes all 14 scientific evaluation modules
        │
        ├──────────────> Integrates with [ sentinel-matrix ]: Samples RK4 ODE physical plant states (mock_plc)
        │
        ├──────────────> Evaluates [ blackbox-sentinel ]: Benchmarks ModelRouter & ThreatCorrelator interlocks
        │
        ├──────────────> Benchmarks [ blackbox-essential ]: Measures eBPF xdp_drop.o & AF_XDP UMEM speed
        │
        ├──────────────> Evaluates [ xinfer-forge ]: Benchmarks Tabular MAE models & Golden Gate safety gates
        │
        └──────────────> Benchmarks [ xinfer-essential ]: Measures FP32 vs INT8 precision drift across 15 backends
```

---

## 8. Academic Citation

If you use `sentinel-lab` or the SLAB wire protocol in academic research, grants, or publications, please cite the permanent Zenodo preprint:

```bibtex
@article{saberifard2026sentinel,
  author    = {Saberifard, Kamran},
  title     = {Sub-Microsecond Edge Active Defense and Hardware-Aware Research Testbeds for Cyber-Physical Systems},
  journal   = {CERN / Zenodo Digital Repository},
  year      = {2026},
  doi       = {10.5281/zenodo.10892341},
  url       = {https://doi.org/10.5281/zenodo.10892341}
}
```

---

## 9. License

Copyright © 2026 **Aryorithm Technologies B.V.** (Amsterdam, The Netherlands). All rights reserved.  
Released under the **Aryorithm Commercial & Academic Research License**. For commercial licensing, contact `licensing@aryorithm.com`.
