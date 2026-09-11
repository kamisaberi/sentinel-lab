Based on the structure of **`blackbox-sentinel`** (with its 26 modules, 30 commercial plugins, air-gapped web center, and TPM licensing), **`sentinel-lab`** must be a streamlined, research-grade evaluation artifact.

In academic computer systems and security venues (*USENIX Security, NDSS, ACM CCS, SOSP*), a research testbed must satisfy the **Artifact Evaluation (AE)** standard: it must be **reproducible, plug-and-play, decoupled from proprietary commercial code, and focused squarely on measuring scientific metrics**.

---

### Architectural Contrast: `blackbox-sentinel` vs. `sentinel-lab`

```text
BLACKBOX-SENTINEL (Commercial Product Appliance)
├── 26 Subsystem Modules (WAF, EDR, RASP, CWPP, SIEM Core, UEBA, etc.)
├── 30 Dynamic Plugins (SCADA Modbus, S7comm, MAVLink, BACnet, AIS, etc.)
├── Embedded Web Dashboard (Port 8443) & WebSocket Streamer (Port 8444)
└── Hardware TPM 2.0 Licensing Engine & CMMC/ISO Compliance Exporter
                                vs.
SENTINEL-LAB (Academic Evaluation Testbed & Artifact)
├── Zero-Copy Ingestion Core (AF_XDP UMEM C++20 Datapath)
├── Temporal Flow Vectorizer (First 16 Packets: Burst Size, IAT Delta)
├── Linkage to `libblackbox-essential.so` -> `libxinfer.so`
├── Automated Multi-Model Benchmarking Harness (SpectraFlow vs. Baselines)
├── Traffic Replayer & High-Concurrency Rate Generator (10k to 1.25M EPS)
└── Automated LaTeX Table & Matplotlib Figure Generation (One-Click Evaluation)
```

---

### Recommended File Structure for `sentinel-lab`

```text
sentinel-lab/
├── CMakeLists.txt                       # Build script linking libblackbox-essential & libxinfer
├── LICENSE                              # Open-Source License (e.g., Apache 2.0 or MIT)
├── README.md                            # Academic Overview, Badges, and Quickstart
├── INSTALL.md                           # Step-by-step Artifact Evaluation Setup Guide
│
├── bpf/                                 # In-Kernel eBPF / XDP Datapath
│   ├── CMakeLists.txt
│   ├── xdp_drop.c                       # Wire-speed packet dropper hook
│   ├── bpf_common.h                     # BPF map definitions (blocked_ip_map)
│   └── Makefile                         # Clang bytecode compilation target
│
├── include/                             # Public C++20 Headers
│   └── sentinel_lab/
│       ├── lab_engine.hpp               # Master testbed coordinator
│       ├── af_xdp_socket.hpp            # Zero-copy UMEM & AF_XDP ring buffer wrapper
│       ├── flow_vectorizer.hpp          # Extracts (d_i * s_i, Delta t_i) for N=16 packets
│       ├── traffic_replayer.hpp         # High-precision line-rate packet replay engine
│       ├── benchmark_harness.hpp        # Profiles model inference, latency & memory
│       └── metrics_collector.hpp        # Lock-free P50/P95/P99 latency & EPS counters
│
├── src/                                 # Implementation Source Files
│   ├── main.cpp                         # CLI executable entry point (sentinel-lab-cli)
│   ├── lab_engine.cpp                   # Testbed execution orchestration
│   ├── af_xdp_socket.cpp                # Native Linux XDP socket driver implementation
│   ├── flow_vectorizer.cpp              # Zero-allocation flow state tracking & tensor packing
│   ├── traffic_replayer.cpp             # Wire-speed PCAP injection pipeline
│   ├── benchmark_harness.cpp            # Model execution loop via blackbox-essential
│   └── metrics_collector.cpp            # Thread-safe timing and percentile calculations
│
├── configs/                             # Experiment Configurations
│   ├── lab_config.json                  # NIC interface, batch size, thread pinning
│   └── experiments/
│       ├── exp1_accuracy.json           # Accuracy validation across datasets
│       ├── exp2_latency.json            # Sub-microsecond latency measurement setup
│       └── exp3_throughput_10g.json     # 10Gbps line-rate stress test configuration
│
├── models/                              # Pre-trained ONNX Models & Baselines
│   ├── README.md                        # Checkpoint hashes, training configs & weights
│   ├── spectraflow_cst.onnx             # Proposed Compact Sequence Transformer (<450k params)
│   └── baselines/
│       ├── fsnet_gru.onnx               # Recurrent Baseline (Bi-GRU)
│       ├── 1d_cnn_wang.onnx             # Convolutional Baseline (1D-CNN)
│       ├── flowpic_resnet.onnx          # Visual FlowPic Baseline (2D-CNN)
│       └── nettisa_mlp.onnx             # Fast Statistical Baseline (NetTiSA MLP)
│
├── datasets/                            # Dataset Ingestion & Preprocessing
│   ├── download_datasets.sh             # Automated script to fetch USTC, CIRA-DoH, CTU-13
│   ├── verify_checksums.sh              # Validates dataset integrity via SHA-256
│   └── sample_traces/                   # Minimal sample PCAPs for rapid CI testing
│       ├── sample_benign_tls13.pcap
│       └── sample_cobalt_strike_c2.pcap
│
├── evaluation/                          # Automated Thesis / Paper Figure & Table Exporters
│   ├── run_all_experiments.sh           # One-click Master's artifact evaluation runner
│   ├── plot_latency_cdf.py              # Generates Figure: Latency Percentile CDF (0.12 - 1.05 us)
│   ├── plot_throughput_scaling.py       # Generates Table: Sustained EPS vs. CPU/RAM Footprint
│   └── generate_confusion_matrices.py   # Generates Table: Precision, Recall, F1-Score per class
│
├── deploy/                              # Environment Setup & Virtual Testbeds
│   ├── Dockerfile                       # Self-contained container (Ubuntu 24.04, clang-18, libbpf)
│   ├── docker-compose.yml               # Two-node virtual network (Traffic Generator <-> Sentinel-Lab)
│   └── setup_veth_testbed.sh            # Virtual kernel network setup (for laptops without 10GbE NICs)
│
└── tests/                               # Test Suite
    ├── CMakeLists.txt
    ├── test_af_xdp.cpp                  # Validates kernel driver binding and UMEM allocations
    ├── test_flow_vectorizer.cpp         # Verifies 16-packet tensor extraction against ground truth
    └── test_blackbox_linkage.cpp        # Verifies integration with libblackbox-essential.so
```

---

### Key Components Explained

#### 1. `bpf/xdp_drop.c` (The Hardware Enforcer)
This is the minimal, stripped-down eBPF program that compiles into `xdp_drop.o`. It contains only:
* The `blocked_ip_map` (a BPF hash table), and
* The XDP packet hook that checks incoming IPv4 source addresses and returns `XDP_DROP` if matched, or `XDP_PASS` if clean.

#### 2. `include/sentinel_lab/flow_vectorizer.hpp` (The Feature Extractor)
Implements your mathematical tensor extraction:
* Maintains a pre-allocated, fixed-size flow table in memory.
* For each observed 5-tuple, stores up to 16 packet events: $\mathbf{p}_i = [d_i \cdot s_i, \; \ln(\Delta t_i + 1.0)]$.
* Once the 16th packet is observed, passes the pointer directly to `libblackbox-essential.so` without intermediate memory allocations.

#### 3. `src/benchmark_harness.cpp` (The Model Profiler)
Calls the inference method exposed by `libblackbox-essential.so` (which calls `libxinfer.so`). It runs through your model collection:
1. `spectraflow_cst.onnx`
2. `fsnet_gru.onnx`
3. `1d_cnn_wang.onnx`
4. `nettisa_mlp.onnx`

It automatically calculates timing per pass using the CPU's high-resolution invariant time-stamp counter (`rdtsc`), outputting the **Min, Mean, P95, and P99 latency percentiles**.

#### 4. `evaluation/run_all_experiments.sh` (The Artifact Badge Guarantee)
Top academic conferences award artifact badges (**Artifact Available**, **Artifact Evaluated**, **Results Reproduced**). Having a single script that runs the experiments, parses the log files, and produces the exact LaTeX tables and PDF graphs for your paper is what distinguishes an elite systems dissertation from standard student projects.

#### 5. `deploy/setup_veth_testbed.sh` (Portability)
Not every reviewer or university professor testing your code will have an Intel X520 10GbE dual-port network card in their workstation. This script sets up a simulated kernel testbed using Linux `veth` (virtual Ethernet) pairs with XDP driver emulation, allowing `sentinel-lab` to be tested on standard laptops and GitHub CI runners.

---

### How It Fits into Your Workflow

1. **`xinfer`:** You build and test the universal AI runtime (ONNX + OpenVINO/TensorRT).
2. **`blackbox-essential`:** You build the security engine (flow management, model registry, eBPF drop mapping).
3. **`sentinel-lab`:** You build this research testbed repository to execute your experimental benchmarks and generate the data for your Master's thesis.
4. **`blackbox-sentinel`:** You keep as your proprietary commercial platform, integrating the proven SpectraFlow engine into your enterprise appliance.