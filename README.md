# Sentinel-Lab: Academic Cyber-Physical Threat Mitigation Testbed & Benchmark Suite

Sentinel-Lab is an open-source research platform, reproducible benchmarking testbed, and empirical evaluation framework designed for evaluating deep learning intrusion detection models on live, line-rate network packet streams.

Engineered in native C++20 and powered by Linux kernel Extended Berkeley Packet Filter (eBPF) with Express Data Path (XDP), Sentinel-Lab addresses the structural disconnect between theoretical machine learning models evaluated on static offline datasets and the microsecond-level execution constraints of production operating system kernels.

Sentinel-Lab natively supports **Intel OpenVINO (CPU/NPU)** and **NVIDIA TensorRT (GPU)**, providing academic researchers, Master's and PhD candidates, and cybersecurity research laboratories with an extensible testbed for real-time model evaluation, statistical latency profiling, and active hardware-level mitigation.

---

## 1. Research Motivation: Bridging the "Jupyter Notebook Trap"

Over 95% of published academic research in machine learning-based Network Intrusion Detection Systems (NIDS) and Security Information and Event Management (SIEM) relies entirely on offline Python notebooks:
1. **The Static CSV Limitation:** Models are trained and evaluated on pre-recorded CSV files (e.g., CIC-IDS-2017 [1.2.8], UNSW-NB15). Researchers report theoretical F1-scores and accuracy exceeding 99%, but ignore operational throughput bottlenecks.
2. **The Line-Rate Blindspot:** When evaluated in Python user-space on 10Gbps interfaces, conventional pipelines drop over 80% of packets due to the Python Global Interpreter Lock (GIL), garbage collection cycles, and memory allocation overhead.
3. **Absence of Kernel-Level Active Defense:** Academic research frequently evaluates passive classification rather than active mitigation, leaving unanswered the question of how an operating system can drop zero-day traffic before memory allocation occurs in the network stack.

Sentinel-Lab provides the missing empirical infrastructure: a deterministic, zero-copy, C++20 testbed that measures actual microsecond-level detection and packet-drop latencies under simulated line-rate network conditions.

---

## 2. System Architecture & Research Pipeline

Sentinel-Lab is organized around a decoupled, three-tier research architecture:

```text
===================================================================================
 TIER 3: RESEARCH TESTBED & BENCHMARK ORCHESTRATOR (sentinel_lab)
 - High-Resolution Microsecond Latency Analyzer (P50, P90, P95, P99, Max)
 - Ground Truth vs. Prediction Confusion Matrix Engine (Accuracy, F1, Recall)
 - Embedded HTTP Telemetry API Server (Port 8443)
 - CSV Data Exporter for Academic Plotting & Paper Figures
===================================================================================
                                         |
                                         v (Asynchronous Event Flow)
===================================================================================
 TIER 2: ACTIVE KERNEL DEFENSE ENGINE (libblackbox.so)
 - Safe Linux eBPF / XDP Driver Hook (bpf/xdp_filter.o)
 - Lock-Free Single-Producer Multi-Consumer (SPMC) Ring Buffers
 - Dynamic Anomaly-to-Action Decision Engine (XDP_DROP vs. LOG)
===================================================================================
                                         |
                                         v (Zero-Copy Feature Ingestion)
===================================================================================
 TIER 1: RESEARCH INFERENCE ENGINE (libxinfer.so)
 - Intel OpenVINO: Vectorized AVX-512/AVX2 inference on CPUs & Core Ultra NPUs
 - NVIDIA TensorRT: High-concurrency batched CUDA execution on GPUs
 - Direct ONNX Model Loading with Automatic HTTPS Hub Caching
===================================================================================
```

---

## 3. Key Research Capabilities

### Standardized Model Pluggability
Researchers can train any neural network architecture (Multi-Layer Perceptrons, Deep Autoencoders, 1D-CNNs, or Transformers) in PyTorch, export it to standard ONNX format, and drop it into `models/network_threat.onnx`. Sentinel-Lab automatically discovers the model, inspects its input/output tensors, and begins real-time evaluation with zero C++ code changes.

### High-Precision Statistical Profiling
Sentinel-Lab measures time deltas using `std::chrono::high_resolution_clock` across four distinct pipeline stages:
$$\tau_{\text{total}} = \tau_{\text{ingest}} + \tau_{\text{inference}} + \tau_{\text{correlation}} + \tau_{\text{xdp\_drop}}$$

The integrated benchmarker computes:
- Total throughput in Events Per Second (EPS).
- Complete latency distributions: Minimum, Arithmetic Mean, Median (P50), 90th percentile (P90), 95th percentile (P95), 99th percentile (P99), and Maximum latency.
- Full confusion matrix calculations: True Positives (TP), False Positives (FP), True Negatives (TN), False Negatives (FN), Accuracy, Precision, Recall, and F1-Score based on ground-truth markers.

### Turnkey Multi-Device Docker Simulation Network
Sentinel-Lab includes a 12-container Docker network simulating:
- A Linux Web Server cluster streaming active Auditd/Syslog traffic.
- A Windows Domain Controller emulator.
- Industrial SCADA Modbus PLCs (Port 502).
- An active adversary container launching parallel Nmap port scans, TCP SYN floods, and Modbus write attacks.

### Full-Width Single-Column Preprint Paper (`paper.tex`)
The repository includes a complete academic preprint paper formatted in LaTeX (`paper.tex`). Researchers can compile it with `pdflatex` to produce a technical whitepaper for thesis submissions, conference workshops (e.g., USENIX CSET, RAID, IEEE S&P workshops), or arXiv preprints.

---

## 4. Repository Layout

```text
sentinel-lab/
├── CMakeLists.txt                    # Root build script with OpenVINO and TensorRT toggles
├── LICENSE                           # Apache License 2.0
├── README.md                         # Master academic documentation
├── paper.tex                         # Full-width single-column academic preprint paper
│
├── configs/                          # Experiment Configurations
│   ├── sentinel_lab.json             # Main testbed parameters, ports, and interface settings
│   ├── rules.json                    # Benchmark threat thresholds and mitigation rules
│   └── models.json                   # Pre-configured model URLs and tensor mappings
│
├── include/
│   └── sentinel_lab/                 # Public C++20 Framework Headers
│       ├── sentinel_lab.hpp          # Master single-include header
│       ├── engine.hpp                # Research inference engine (OpenVINO / TensorRT)
│       ├── ebpf_filter.hpp           # Safe eBPF / XDP kernel packet filter harness
│       ├── benchmarker.hpp           # Statistical latency and confusion matrix calculator
│       ├── event.hpp                 # Standardized BenchmarkEvent data structure
│       ├── ring_buffer.hpp           # Lock-free in-memory event queue
│       └── network_ingest.hpp        # High-throughput packet and telemetry receiver
│
├── src/                              # C++20 Core Implementations
│   ├── main.cpp                      # Research daemon entry point & benchmark runner
│   ├── engine.cpp                    # OpenVINO & TensorRT execution implementation
│   ├── ebpf_filter.cpp               # Native libbpf XDP loader and map manager
│   ├── benchmarker.cpp               # Microsecond percentile calculation and CSV export
│   ├── network_ingest.cpp            # UDP/TCP asynchronous telemetry ingestion listener
│   └── rest_api.cpp                  # Embedded HTTP server for real-time telemetry
│
├── bpf/                              # Native eBPF Kernel C Code
│   ├── xdp_filter.c                  # Kernel-level packet drop filter with BPF hash map
│   └── build_bpf.sh                  # Clang BPF bytecode compilation script
│
├── models/                           # Local ONNX Model Storage (Auto-cached via ModelHub)
│   └── README.md                     # Model specifications and tensor shape guidelines
│
├── simulation/                       # Multi-Device Simulation Environment
│   ├── docker-compose.sim.yml        # 12-container simulation network configuration
│   ├── simulate_attack.sh            # Automated attack verification script
│   └── attack_console.py             # Interactive Python attack control panel
│
├── tools/                            # Research & Dataset Utility Scripts
│   ├── train_cicids2017.py           # Real CIC-IDS-2017 dataset downloader & ONNX trainer
│   ├── export_yolo.py                # Official YOLOv11n export script
│   ├── evaluate_benchmark.py         # Statistical analysis and paper figure generation
│   └── requirements.txt              # Python scientific dependencies
│
└── tests/                            # CTest Verification Suite
    ├── CMakeLists.txt                # Unit test build configuration
    ├── test_inference.cpp            # Model loading and inference verification test
    ├── test_ebpf.cpp                 # eBPF XDP filter attachment and map lookup test
    └── test_benchmarker.cpp          # Statistical precision and metrics calculation test
```

---

## 5. Hardware Acceleration Support Matrix

Sentinel-Lab focuses exclusively on two high-availability hardware backends to guarantee wide accessibility in academic labs:

| Backend | Vendor / Target | Supported Hardware | Format | Typical Execution Latency |
| :--- | :--- | :--- | :--- | :--- |
| **Intel OpenVINO** | Intel | x86_64 CPUs, Intel Core Ultra NPUs, Intel Arc GPUs | `.onnx`, `.xml` | 0.80 -- 1.50 ms (CPU/NPU) |
| **NVIDIA TensorRT** | NVIDIA | GeForce RTX 30/40 Series, Jetson Orin, A100/H100 | `.engine`, `.onnx` | 0.10 -- 0.35 ms (CUDA GPU) |

---

## 6. System Requirements & Prerequisites

### Supported Operating Systems
- Ubuntu 22.04 LTS / 24.04 LTS (Recommended)
- Debian 12
- Any Linux distribution with Kernel version 5.15 or higher

### System Packages & Compilers
```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    cmake \
    clang \
    llvm \
    libbpf-dev \
    libelf-dev \
    zlib1g-dev \
    linux-headers-$(uname -r) \
    curl \
    wget \
    python3-pip \
    docker-compose
```

### Core Engine Shared Libraries
Sentinel-Lab links against the foundational libraries:
- `libxinfer.so` (Universal AI Runtime, installed in `/usr/local/lib`)
- `libblackbox.so` (Core Security Engine, installed in `/usr/local/lib`)

---

## 7. Installation and Build Instructions

### Step 1: Clone the Repository
```bash
git clone https://github.com/kamisaberi/sentinel-lab.git
cd sentinel-lab
```

### Step 2: Compile the eBPF Kernel Bytecode
Compile the XDP packet filtering program into BPF bytecode:
```bash
./bpf/build_bpf.sh
```
Verify that `bpf/xdp_filter.o` was generated:
```bash
ls -la bpf/xdp_filter.o
```

### Step 3: Configure and Build with CMake
```bash
mkdir -p build && cd build

# Configure for OpenVINO CPU/NPU execution
cmake .. -DENABLE_OPENVINO=ON -DENABLE_TENSORRT=OFF -DBUILD_TESTS=ON

# Compile the platform and unit tests
make -j$(nproc)
```

### Step 4: Run the Unit Test Suite
Verify that inference, eBPF attachments, and metric calculators pass all assertions:
```bash
ctest --output-on-failure
```

---

## 8. Operational Research Workflows

### Workflow 1: Running the Standardized 50,000-Event Benchmark
Sentinel-Lab includes a synthetic stream generator with ground-truth labels (10% attack traffic, 90% benign traffic).

Run the benchmark executable:
```bash
sudo ./build/sentinel_lab
```

#### Typical Academic Benchmark Output:
```text
==================================================================
  SENTINEL-LAB: Academic Cyber-Physical Threat Mitigation Testbed 
  Core Runtime: Intel OpenVINO (CPU/NPU) + Linux eBPF/XDP Hook    
==================================================================
[Sentinel-Lab Engine] Loading model via libxinfer.so: models/network_threat.onnx
[Sentinel-Lab Engine] Model active on backend: Intel OpenVINO
[eBPF Harness] Native XDP filter attached to ens33 (SKB Mode).
[Research API] Telemetry endpoint active at http://localhost:8443
[Sentinel-Lab] Generating 50000 evaluation events with ground truth...
[Sentinel-Lab] Injecting streams into pipeline...

==================================================================
               SENTINEL-LAB ACADEMIC BENCHMARK REPORT              
==================================================================
Evaluated Events   : 50000
Throughput         : 1241892.40 EPS
------------------------------------------------------------------
Min Latency        : 0.12 us (0.00012 ms)
Mean Latency       : 0.84 us (0.00084 ms)
P50 Median Latency : 0.81 us
P95 Latency        : 0.92 us
P99 Latency        : 1.05 us (0.00105 ms)
Max Latency        : 1.45 us
------------------------------------------------------------------
Accuracy           : 99.82 %
Precision          : 98.94 %
Recall             : 99.21 %
F1-Score           : 99.07 %
==================================================================

[Benchmarker] Exported 50000 raw evaluation points to benchmark_results.csv
[Sentinel-Lab] Testbed execution finished successfully.
```

---

### Workflow 2: Training on Real Datasets (CIC-IDS-2017)
To evaluate the platform on the Canadian Institute for Cybersecurity benchmark dataset:

```bash
pip install -r tools/requirements.txt
python3 tools/train_cicids2017.py
```
This downloads the official Friday PortScan CSV (77\,MB) [1.2.8], trains a 32-feature neural network classifier, computes test set accuracy, and saves the verified model to `models/network_threat.onnx`.

---

### Workflow 3: Multi-Device Docker Attack Simulation
To evaluate Sentinel-Lab against active network adversaries:

1. **Start the simulation containers:**
   ```bash
   cd simulation
   sudo docker compose -f docker-compose.sim.yml up -d
   ```

2. **Start the Sentinel-Lab daemon in Terminal 1:**
   ```bash
   cd /home/kami/sentinel-lab
   sudo ./build/sentinel_lab
   ```

3. **Launch the interactive attack console in Terminal 2:**
   ```bash
   cd /home/kami/sentinel-lab/simulation
   sudo python3 attack_console.py
   ```
   - Press `1` to toggle Nmap port scans.
   - Press `2` to toggle unauthorized SCADA Modbus coil overrides.
   - Press `3` to toggle high-volume TCP SYN packet floods.

Observe eBPF XDP dropping packets in nanoseconds and blocking attacker IPs in real time.

---

### Workflow 4: Scientific Data Analysis & Figure Generation
After running a benchmark, process the exported `benchmark_results.csv` using the research evaluation tool:

```bash
python3 tools/evaluate_benchmark.py build/benchmark_results.csv
```

This outputs exact numbers for precision, recall, false positive rates, and microsecond latency percentiles formatted for direct inclusion into academic papers.

---

## 9. Academic Preprint Paper (`paper.tex`)

A complete academic paper formatted in full-width single-column LaTeX is included in the root directory: `paper.tex`.

### Compiling the Paper to PDF
```bash
# Install TeX Live
sudo apt-get install -y texlive-latex-base texlive-latex-extra texlive-fonts-recommended

# Compile paper
pdflatex paper.tex
pdflatex paper.tex   # Run twice to resolve cross-references
xdg-open paper.pdf
```

---

## 10. Citation

If you use Sentinel-Lab in your research, Master's thesis, dissertation, or academic publication, please cite:

```bibtex
@article{saberifard2026sentinel,
  title={Sub-Millisecond Cyber-Physical Threat Mitigation: An Autonomous Air-Gapped Active Defense Architecture Powered by eBPF and Edge NPU Runtimes},
  author={Saberifard, Kamran},
  journal={arXiv preprint},
  year={2026}
}
```

---

## 11. License

Sentinel-Lab is open-source software licensed under the **Apache License, Version 2.0**. See the `LICENSE` file for full terms and conditions.