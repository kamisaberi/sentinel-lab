Here is the recommended, production-ready repository architecture for **`sentinel-lab`** (`https://github.com/kamisaberi/sentinel-lab`).

`sentinel-lab` serves as the **open-source academic research platform and reproducible benchmark testbed** (supporting **Intel OpenVINO** and **NVIDIA TensorRT**). It is designed so that Master's/PhD students, professors, and cybersecurity researchers can clone it, drop in their ONNX models, and immediately measure microsecond-level latency and line-rate throughput under live attack traffic.

---

### Complete Repository Structure for `sentinel-lab`

```text
sentinel-lab/
├── CMakeLists.txt                    # Root CMake build configuration (OpenVINO + TensorRT)
├── LICENSE                           # Open-Source License (Apache 2.0 or MIT)
├── README.md                         # Academic & Research documentation
├── paper.tex                         # Full-width single-column academic preprint paper
│
├── configs/                          # Research & Evaluation Configurations
│   ├── sentinel_lab.json             # Main lab testbed configuration
│   ├── rules.json                    # Benchmark detection and correlation rules
│   └── models.json                   # Pre-configured Hugging Face / ONNX Model Zoo URLs
│
├── include/
│   └── sentinel_lab/                 # Public C++20 Research Framework Headers
│       ├── sentinel_lab.hpp          # Master library include header
│       ├── engine.hpp                # OpenVINO & TensorRT inference wrapper
│       ├── ebpf_filter.hpp           # Safe eBPF / XDP kernel packet filter harness
│       ├── benchmarker.hpp           # High-resolution latency (P50/P95/P99) & EPS calculator
│       ├── event.hpp                 # Standardized SecurityEvent data structure
│       └── ring_buffer.hpp           # Lock-free in-memory event queue
│
├── src/                              # C++20 Testbed Implementation Source
│   ├── main.cpp                      # Sentinel-Lab daemon entry point
│   ├── engine.cpp                    # Inference dispatch (OpenVINO CPU/NPU + TensorRT GPU)
│   ├── ebpf_filter.cpp               # eBPF XDP bytecode loader & BPF map manager
│   ├── benchmarker.cpp               # Microsecond latency distribution analyzer
│   ├── network_ingest.cpp            # Raw socket / PCAP replay / test packet receiver
│   └── rest_api.cpp                  # Embedded HTTP server for real-time telemetry
│
├── bpf/                              # Native eBPF Kernel C Code
│   ├── xdp_filter.c                  # Kernel-level packet filter & BPF hash map
│   └── build_bpf.sh                  # Clang BPF bytecode compilation script
│
├── models/                           # Local ONNX Model Cache (Auto-fetched via ModelHub)
│   └── README.md                     # Instructions on supported ONNX models
│
├── simulation/                       # Turnkey Multi-Device Docker Simulation Network
│   ├── docker-compose.sim.yml        # Multi-container network (Web cluster, SCADA, Attacker)
│   ├── simulate_attack.sh            # One-shot attack verification script
│   └── attack_console.py             # Interactive Python attack control panel
│
├── tools/                            # Python Research & Dataset Utility Scripts
│   ├── train_cicids2017.py           # Real CIC-IDS-2017 dataset downloader & ONNX trainer
│   ├── export_yolo.py                # Official YOLOv11n export script
│   ├── evaluate_benchmark.py         # Pulls metrics from API & plots latency/ROC figures
│   └── requirements.txt              # Python research dependencies
│
└── tests/                            # CTest Research Verification Suite
    ├── CMakeLists.txt                # Tests build script
    ├── test_inference.cpp            # OpenVINO/TensorRT inference unit test
    ├── test_ebpf.cpp                 # XDP filter attach/detach test
    └── test_benchmarker.cpp          # Metric precision and statistical test
```

---

### Key Components to Put in Place First

#### 1. Root `CMakeLists.txt`
This root build file focuses on **OpenVINO** (default ON for CPU/NPU) and **TensorRT** (optional for NVIDIA GPUs):

```cmake
cmake_minimum_required(VERSION 3.20)
project(sentinel-lab VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Research Hardware Toggles (Default: OpenVINO ON, TensorRT optional)
option(ENABLE_OPENVINO "Enable Intel OpenVINO Backend (CPU/NPU)" ON)
option(ENABLE_TENSORRT "Enable NVIDIA TensorRT Backend (GPU)"     OFF)
option(BUILD_TESTS     "Build unit tests and benchmarks"          ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/local/include
)

# Core System Dependencies
find_package(Threads REQUIRED)
find_library(BPF_LIB bpf REQUIRED)
find_library(ELF_LIB elf REQUIRED)
find_library(XINFER_LIB xinfer REQUIRED PATHS /usr/local/lib /usr/lib)
find_library(BLACKBOX_LIB blackbox REQUIRED PATHS /usr/local/lib /usr/lib)

set(LAB_SOURCES
    src/main.cpp
    src/engine.cpp
    src/ebpf_filter.cpp
    src/benchmarker.cpp
    src/network_ingest.cpp
    src/rest_api.cpp
)

if(ENABLE_OPENVINO)
    add_compile_definitions(SENTINEL_LAB_OPENVINO)
    find_package(OpenVINO REQUIRED)
endif()

if(ENABLE_TENSORRT)
    enable_language(CUDA)
    add_compile_definitions(SENTINEL_LAB_TENSORRT)
    find_package(CUDA REQUIRED)
    find_library(TENSORRT_NVINFER nvinfer REQUIRED)
    include_directories(${CUDA_INCLUDE_DIRS})
endif()

add_executable(sentinel_lab ${LAB_SOURCES})

target_link_libraries(sentinel_lab PRIVATE
    Threads::Threads
    ${BPF_LIB}
    ${ELF_LIB}
    z
    ${XINFER_LIB}
    ${BLACKBOX_LIB}
)

if(ENABLE_OPENVINO)
    target_link_libraries(sentinel_lab PRIVATE openvino::runtime)
endif()

if(ENABLE_TENSORRT)
    target_link_libraries(sentinel_lab PRIVATE ${CUDA_LIBRARIES} ${TENSORRT_NVINFER})
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

---

#### 2. Master Research `README.md` for GitHub

```markdown
# Sentinel-Lab: Academic Cyber-Physical Threat Mitigation Testbed

Sentinel-Lab is an open-source research platform and reproducible benchmarking testbed designed for evaluating machine learning intrusion detection models under real-time, line-rate network execution. 

Built in native C++20 and powered by Linux kernel eBPF/XDP and the `xinfer` engine, Sentinel-Lab allows researchers and students to evaluate ONNX deep learning models on live packet streams and measure true microsecond-level detection and packet-drop latencies.

## Core Capabilities
- **Universal Inference Support:** Native hardware execution on commodity CPUs and Intel NPUs via OpenVINO, with optional NVIDIA TensorRT acceleration.
- **Kernel-Level eBPF/XDP Mitigation:** Drops malicious packets at the network interface driver level in nanoseconds.
- **Turnkey Simulation Testbed:** 12-container Docker network simulating enterprise servers, SCADA Modbus PLCs, and active multi-vector attackers.
- **Reproducible Academic Benchmarking:** Computes high-precision latency percentiles (Min, Mean, P95, P99) and sustained throughput (Events Per Second).

## Academic Preprint Paper
A complete academic paper detailing the testbed architecture, eBPF implementation, and comparative benchmarks against legacy SIEM platforms is included in `paper.tex`.

## Quickstart Guide

### 1. Install Prerequisites (Ubuntu 22.04 / 24.04 LTS)
```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential cmake clang llvm libbpf-dev libelf-dev zlib1g-dev \
    openvino-2024.0.0 curl wget python3-pip
```

### 2. Build the Platform
```bash
mkdir build && cd build
cmake .. -DENABLE_OPENVINO=ON
make -j$(nproc)
```

### 3. Run the Testbed & Simulation
```bash
# Terminal 1: Launch the research daemon
sudo ./sentinel_lab

# Terminal 2: Launch the interactive attack simulation
cd simulation
sudo docker compose -f docker-compose.sim.yml up -d
sudo python3 attack_console.py
```

## Citation
If you use Sentinel-Lab in your research, Master's thesis, or publication, please cite:
```bibtex
@article{saberifard2026sentinel,
  title={Sub-Millisecond Cyber-Physical Threat Mitigation: An Autonomous Air-Gapped Active Defense Architecture Powered by eBPF and Edge NPU Runtimes},
  author={Saberifard, Kamran},
  journal={arXiv preprint},
  year={2026}
}
```

## License
Licensed under the Apache License, Version 2.0.
```

---

### Step-by-Step Commands to Initialize Your Repository

Open your terminal on your Ubuntu machine to set up the local repository and push it to GitHub:

```bash
# 1. Clone your empty repository from GitHub
cd /home/kami
git clone https://github.com/kamisaberi/sentinel-lab.git
cd sentinel-lab

# 2. Create the complete directory structure
mkdir -p configs include/sentinel_lab src bpf models simulation tools tests

# 3. Create the placeholder files
touch CMakeLists.txt README.md LICENSE paper.tex
touch configs/sentinel_lab.json configs/rules.json configs/models.json
touch bpf/xdp_filter.c bpf/build_bpf.sh
touch simulation/docker-compose.sim.yml simulation/simulate_attack.sh simulation/attack_console.py
touch tools/train_cicids2017.py tools/export_yolo.py tools/evaluate_benchmark.py tools/requirements.txt
touch tests/CMakeLists.txt tests/test_inference.cpp tests/test_ebpf.cpp tests/test_benchmarker.cpp

# 4. Copy your paper.tex into the root
cp /home/kami/blackbox-sentinel/paper.tex ./paper.tex

# 5. Commit and push your initial structure
git add .
git commit -m "Initialize Sentinel-Lab research testbed architecture"
git branch -M main
git push -u origin main
```

Your repository will be structured, clearly separated from the commercial product, and ready for you to add code.