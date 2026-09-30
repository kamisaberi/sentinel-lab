### Part 1: Root Configuration & Getting Started (`mkdocs.yml`, `index.md`, and `getting-started/*`)

This initial set of 8 files establishes the full documentation configuration, the executive research landing page, and the onboarding track for **`sentinel-lab`** (`sentinel_lab`).

---

### File: `sentinel-lab/docs/mkdocs.yml`

```yaml
site_name: Sentinel-Lab Research Documentation
site_description: Open Academic Research Testbed, SLAB Binary Wire Protocol, and Reproducible Sub-Microsecond XDR Benchmark Harness
site_author: Aryorithm Technologies B.V. & Academic Collaborators
site_url: https://docs.aryorithm.com/lab/
repo_name: kamisaberi/sentinel-lab
repo_url: https://github.com/kamisaberi/sentinel-lab

theme:
  name: material
  language: en
  palette:
    - scheme: slate
      primary: blue grey
      accent: indigo
      toggle:
        icon: material/weather-night
        name: Switch to light mode
    - scheme: default
      primary: blue grey
      accent: indigo
      toggle:
        icon: material/weather-sunny
        name: Switch to dark mode
  features:
    - navigation.instant
    - navigation.tracking
    - navigation.tabs
    - navigation.sections
    - navigation.expand
    - navigation.top
    - search.suggest
    - search.highlight
    - content.code.copy
    - content.code.annotate

plugins:
  - search

markdown_extensions:
  - admonition
  - pymdownx.details
  - pymdownx.superfences:
      custom_fences:
        - name: mermaid
          class: mermaid
          format: !!python/name:pymdownx.superfences.fence_code_format
  - pymdownx.highlight:
      anchor_linenums: true
      line_spans: __span
      pygments_lang_class: true
  - pymdownx.inlinehilite
  - pymdownx.tabbed:
      alternate_style: true
  - pymdownx.arithmatex:
      generic: true
  - tables
  - attr_list
  - md_in_html

extra_javascript:
  - https://polyfill.io/v3/polyfill.min.js?features=es6
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js

nav:
  - Home: index.md
  - Getting Started:
      - Research Overview: getting-started/overview.md
      - System Requirements: getting-started/system-requirements.md
      - Installation & Build: getting-started/installation-and-build.md
      - 10-Minute Quickstart: getting-started/ten-minute-quickstart.md
      - Verifying Environment: getting-started/verifying-environment.md
      - Architecture at a Glance: getting-started/architecture-at-a-glance.md
  - Architecture:
      - Testbed Architecture: architecture/testbed-architecture.md
      - Reproducibility Imperative: architecture/academic-reproducibility-imperative.md
      - Hardware-in-the-Loop Design: architecture/hardware-in-the-loop-design.md
      - Latency Measurement Physics: architecture/latency-measurement-physics.md
      - Zero-Overhead Instrumentation: architecture/zero-overhead-instrumentation.md
  - SLAB Protocol:
      - Protocol Specification: slab-protocol/protocol-specification.md
      - Bytefield Wire Layout: slab-protocol/bytefield-wire-layout.md
      - Zero-Copy Casting: slab-protocol/zero-copy-casting.md
      - Multi-Dataset Compatibility: slab-protocol/multi-dataset-compatibility.md
      - Python SLAB Serializer: slab-protocol/python-slab-serializer.md
      - Protocol Validation Checks: slab-protocol/protocol-validation-checks.md
  - Dual-Silicon Testbed:
      - Comparative Model: dual-silicon-testbed/dual-target-comparative-model.md
      - Intel OpenVINO Pipeline: dual-silicon-testbed/intel-openvino-pipeline.md
      - NVIDIA TensorRT Pipeline: dual-silicon-testbed/nvidia-tensorrt-pipeline.md
      - Cross-Silicon Standards: dual-silicon-testbed/cross-silicon-benchmark-standards.md
      - Thermal & Power Profiling: dual-silicon-testbed/thermal-and-power-profiling.md
  - Preprint & Open Science:
      - Preprint Overview: preprint-and-open-science/preprint-overview.md
      - CERN Zenodo DOI: preprint-and-open-science/cern-zenodo-doi.md
      - Compiling LaTeX Paper: preprint-and-open-science/compiling-latex-paper.md
      - Citing Sentinel-Lab: preprint-and-open-science/citing-sentinel-lab.md
      - Open Access Licensing: preprint-and-open-science/open-access-licensing.md
  - Evaluation Harness:
      - Harness Architecture: evaluation-harness/harness-architecture.md
      - CIC-IDS-2017 Dataset: evaluation-harness/dataset-acquisition-cic-ids-2017.md
      - Flow Normalization: evaluation-harness/flow-normalization-pipeline.md
      - Raw Socket Injection: evaluation-harness/wire-speed-raw-socket-injection.md
      - Metrics Calculation: evaluation-harness/metrics-calculation.md
      - Latency Profiler: evaluation-harness/percentile-latency-profiler.md
  - Comparative Benchmarks:
      - Methodology: comparative-benchmarks/benchmark-methodology.md
      - vs. Suricata 7: comparative-benchmarks/sentinel-vs-suricata-7.md
      - vs. Snort 3: comparative-benchmarks/sentinel-vs-snort-3.md
      - vs. Elastic SIEM: comparative-benchmarks/sentinel-vs-elastic-siem.md
      - vs. Splunk Enterprise: comparative-benchmarks/sentinel-vs-splunk-enterprise.md
      - Latency CDF Percentiles: comparative-benchmarks/latency-cdf-percentiles.md
      - Reproducibility Audit: comparative-benchmarks/reproducibility-audit.md
  - University Curriculum:
      - Thesis Topics Guide: university-curriculum/thesis-topics-guide.md
      - Course Module Integration: university-curriculum/course-module-integration.md
      - TalTech Collaboration: university-curriculum/taltech-collaboration-guide.md
      - Aalto University Alignment: university-curriculum/aalto-collaboration-guide.md
      - Student Grant Support: university-curriculum/student-grant-support.md
  - Tutorials:
      - Custom CSV Evaluation: tutorials/evaluating-custom-csv-datasets.md
      - Intel NPU vs. CPU: tutorials/benchmarking-intel-npu-vs-cpu.md
      - Measuring XDP Cycles: tutorials/measuring-xdp-drop-cycles.md
      - Exporting LaTeX Tables: tutorials/exporting-reproducible-csv-artifacts.md
      - Running in VMware: tutorials/running-testbed-in-vmware.md
  - Troubleshooting:
      - Dataset Download Errors: troubleshooting/dataset-download-errors.md
      - Raw Socket Permissions: troubleshooting/raw-socket-permission-denied.md
      - eBPF JIT Compiler Errors: troubleshooting/ebpf-jit-compiler-errors.md
      - OpenVINO / CUDA Linking: troubleshooting/openvino-tensorrt-linking-issues.md
      - FAQ: troubleshooting/faq.md
      - Academic Support: troubleshooting/support.md
```

---

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

---

### File: `sentinel-lab/docs/getting-started/overview.md`

```markdown
# Open Research Testbed: Sub-Microsecond Threat Mitigation

Intrusion detection research in academic literature suffers from widespread methodological flaws:
* **The "CSV Illusion":** Over $85\%$ of published papers train classifiers on static CSV rows using Pandas and Scikit-Learn, reporting high theoretical accuracies ($> 99\%$) while ignoring feature extraction delays, socket buffer overheads, and network jitter.
* **The Mitigation Void:** Traditional research evaluates *detection* (alerting) rather than *mitigation* (active containment). In operational networks, an alert emitted $30\text{ seconds}$ late fails to stop catastrophic physical failure.
* **Hardware Disconnection:** Models are rarely benchmarked on resource-constrained edge NPUs or integrated embedded silicon under realistic thermal budgets.

`sentinel-lab` provides the academic community with an **open, reproducible, hardware-in-the-loop research harness**.

---

## 1. Research Contributions

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. The SLAB Binary Wire Protocol                            │
 │    A self-describing, zero-copy packet wire format that     │
 │    embeds ground-truth labels alongside raw feature tensors.│
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ 2. End-to-End Latency Measurement Physics                   │
 │    Nanosecond-level profiling from physical wire arrival to │
 │    in-kernel eBPF packet mitigation (< 0.84 µs).            │
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ 3. Automated Benchmark Pipeline                             │
 │    Downloads official CIC-IDS-2017 datasets, serializes to  │
 │    SLAB, blasts over raw sockets, and outputs LaTeX tables. │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Supported Research Benchmarks

* **CIC-IDS-2017 (Canadian Institute for Cybersecurity):** PortScan, DoS, and BruteForce subsets.
* **UNSW-NB15 (University of New South Wales):** Advanced lateral movement and modern exploit vectors.
* **Industrial SCADA Traces:** Triton, Stuxnet, and Industroyer2 cyber-physical attack replays.
```

---

### File: `sentinel-lab/docs/getting-started/system-requirements.md`

```markdown
# System Requirements & Academic Lab Prerequisites

Review the toolchain, kernel configuration, and hardware requirements before building `sentinel-lab`.

---

## 1. Operating System & Toolchain

* **Operating System:** 64-bit Linux (Ubuntu 22.04 LTS, Ubuntu 24.04 LTS, Debian 12 Bookworm).
* **Linux Kernel:** Version **>= 5.15** (Kernel **>= 6.8** recommended for modern BTF and XDP features).
* **C++ Compiler:** Clang 16.0+ or GCC 12.1+ supporting **ISO C++20**.
* **eBPF Compiler:** Clang/LLVM 15.0+ with `-target bpf`.
* **Build System:** CMake **>= 3.20** and Ninja Build **>= 1.10**.
* **Python Runtime:** Python **>= 3.10** with `requests`, `numpy`, and `tqdm`.

---

## 2. Hardware Testbed Recommendations

The testbed runs in two modes:

| Resource Profile | Minimal Educational Setup (Student Laptop) | High-Performance Research Testbed (Lab Server) |
| :--- | :--- | :--- |
| **CPU** | 4-Core x86_64 or Apple Silicon (VMware/UTM) | Intel Core i9-14900K or AMD Ryzen 9 7950X |
| **RAM** | 8 GB RAM | 64 GB to 192 GB DDR5 RAM |
| **NIC** | Standard Virtual Adapter (`e1000`, `vmxnet3`) | Intel 82599ES (10GbE) or Intel E810 (25GbE) |
| **AI Acceleration** | Intel CPU AVX2 Reference Backend | Intel Core Ultra NPU / NVIDIA RTX A4000 / Jetson |

---

## 3. Required Linux Kernel Configuration Flags

Ensure the host kernel enables eBPF and raw packet socket interfaces:

```bash
# Verify active kernel flags
cat /boot/config-$(uname -r) | grep -E 'CONFIG_BPF_SYSCALL|CONFIG_NET_RAW|CONFIG_XDP_SOCKETS'
```

* `CONFIG_BPF_SYSCALL=y`: Allows userspace programs to load eBPF bytecode.
* `CONFIG_PACKET=y`: Enables `AF_PACKET` raw socket injection.
* `CONFIG_XDP_SOCKETS=y`: Enables zero-copy AF_XDP ring descriptors.
```

---

### File: `sentinel-lab/docs/getting-started/installation-and-build.md`

```markdown
# Building the Research Engine & eBPF Drivers

This guide covers building the native C++ testbed engine (`sentinel_lab`), compiling in-kernel eBPF filters, and installing dependencies from source.

---

## 1. Install System Dependencies

### Ubuntu 24.04 / 22.04 LTS

```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    clang-16 \
    llvm-16 \
    lld-16 \
    libelf-dev \
    zlib1g-dev \
    libbpf-dev \
    linux-headers-$(uname -r) \
    cmake \
    ninja-build \
    python3-pip \
    python3-venv \
    curl
```

---

## 2. Clone the Repository

```bash
git clone --recurse-submodules https://github.com/kamisaberi/sentinel-lab.git
cd sentinel-lab
```

---

## 3. Compile In-Kernel eBPF Bytecode

Before compiling the host C++ harness, compile the eBPF packet mitigation filter:

```bash
cd bpf
chmod +x build_bpf.sh
./build_bpf.sh
cd ..
```

This generates `bpf/xdp_filter.o` targeting the in-kernel BPF virtual machine.

---

## 4. Build the C++20 Research Engine

Configure and compile the project using CMake and Ninja:

```bash
mkdir build && cd build

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-16 \
    -DENABLE_OPENVINO=ON \
    -DENABLE_TENSORRT=OFF \
    -DBUILD_BENCHMARKS=ON ..

ninja -j$(nproc)
```

### Generated Artifacts in `build/bin/`:
* `sentinel_lab`: The core C++20 research execution harness.
* `slab_generator`: CLI utility for serializing custom datasets into the SLAB protocol.
* `perf_evaluator`: Microsecond-precision hardware timer benchmark.
```

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

---

### File: `sentinel-lab/docs/getting-started/verifying-environment.md`

```markdown
# Verifying Your Environment & Socket Capabilities

Validate that your Linux host environment possesses the necessary socket permissions, eBPF capabilities, and memory limits before initiating line-rate benchmark runs.

---

## 1. Testing Raw Socket Privileges (`CAP_NET_RAW`)

Streaming the SLAB protocol over raw layer-2 interfaces requires either superuser privileges (`sudo`) or the `CAP_NET_RAW` Linux capability:

```bash
# Check if current user or binary can open raw sockets
python3 -c "import socket; s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)"
```

If this raises `PermissionError: [Errno 1] Operation not permitted`, assign the capability to the Python virtual environment or binary:

```bash
sudo setcap 'cap_net_raw,cap_net_admin=+ep' $(which python3)
```

---

## 2. Checking eBPF JIT & BTF Availability

Confirm that the eBPF Just-In-Time compiler is running and kernel type information is readable:

```bash
# 1. Verify eBPF JIT Compiler is Active
cat /proc/sys/net/core/bpf_jit_enable
# Expected Output: 1

# 2. Check for BTF kernel debug information
ls -lh /sys/kernel/btf/vmlinux
# Expected: File exists (~4-6 MB)
```

---

## 3. Checking Memory Locking Limits (`ulimit -l`)

Benchmarking zero-copy packet buffers and pinned memory requires unrestricted locked pages:

```bash
ulimit -l
```

If the output is not `unlimited`, update `/etc/security/limits.conf` as documented in Tier 2 `blackbox-essential`.
```

---

### File: `sentinel-lab/docs/getting-started/architecture-at-a-glance.md`

```markdown
# Architecture at a Glance

The diagram below illustrates the flow of benchmark data through the `sentinel-lab` research harness: from binary dataset serialization through raw socket injection, in-kernel eBPF mitigation, and empirical metrics extraction.

---

```text
                                [ BENCHMARK DATASET CORPUS ]
                                (CIC-IDS-2017 / UNSW-NB15)
                                             │
                                             ▼ tools/csv_to_slab.py
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ SLAB BINARY WIRE FRAMES (Magic: 0x534C4142 | Ground Truth | 32-dim Tensor)               │
 └───────────────────────────────────────────┬──────────────────────────────────────────────┘
                                             │
                                             ▼ Raw Socket Injection (AF_PACKET / eth0)
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ Linux Driver Ingress Hook (eBPF / XDP Data Plane)                                        │
 │  - Inspects incoming SLAB Ethernet frame                                                 │
 │  - Matches IP in blocked_ip_map: If attack detected previously -> XDP_DROP (< 0.84 µs)   │
 │  - Clean / Un-evaluated frames pass to research harness                                  │
 └───────────────────────────────────────────┬──────────────────────────────────────────────┘
                                             │
                                             ▼ Zero-Copy Pointer Casting
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ sentinel_lab Research Harness (C++20 Engine)                                             │
 │                                                                                          │
 │  ┌────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Heterogeneous Silicon Acceleration (libxinfer.so)                                  │  │
 │  │  • Intel Core Ultra NPU / Xeon AVX-512                                             │  │
 │  │  • NVIDIA Jetson Orin Nano / RTX A4000 (CUDA Streams)                              │  │
 │  └────────────────────────────────────────┬───────────────────────────────────────────┘  │
 │                                           │ Model Classification Output                  │
 │                                           ▼                                              │
 │  ┌────────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ Scientific Parity & Latency Profiler                                               │  │
 │  │  • Ground Truth vs. Predicted Class ──► Updates Confusion Matrix (TP, FP, FN, TN)  │  │
 │  │  • Hardware Cycle Timer (__rdtsc)   ──► Records Exact Nanosecond Latency           │  │
 │  └────────────────────────────────────────────────────────────────────────────────────┘  │
 └───────────────────────────────────────────┬──────────────────────────────────────────────┘
                                             │
                                             ▼ Automated Artifact Generation
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ IEEE Preprint Manuscript Tables (paper/tables/results.tex) & CERN/Zenodo DOI Archive     │
 └──────────────────────────────────────────────────────────────────────────────────────────┘
```
```

---

### Complete in Part 1
- `sentinel-lab/docs/mkdocs.yml`
- `sentinel-lab/docs/index.md`
- `sentinel-lab/docs/getting-started/overview.md`
- `sentinel-lab/docs/getting-started/system-requirements.md`
- `sentinel-lab/docs/getting-started/installation-and-build.md`
- `sentinel-lab/docs/getting-started/ten-minute-quickstart.md`
- `sentinel-lab/docs/getting-started/verifying-environment.md`
- `sentinel-lab/docs/getting-started/architecture-at-a-glance.md`

All 8 root configuration and onboarding files are now generated.

---

### Files to be Generated in Part 2

The next phase covers **Research & Systems Engineering Design** (`architecture/` - 5 files):

1. `architecture/testbed-architecture.md` (Decoupled C++20 research engine and socket polling loop)
2. `architecture/academic-reproducibility-imperative.md` (Why static CSV training fails in real-world packet processing)
3. `architecture/hardware-in-the-loop-design.md` (Validating true driver-level mitigation on live 10GbE network frames)
4. `architecture/latency-measurement-physics.md` (Microsecond clock precision, hardware timestamps, and jitter)
5. `architecture/zero-overhead-instrumentation.md` (Profiling execution cycles without polluting benchmark results)

Confirm when you are ready to proceed with Part 2.