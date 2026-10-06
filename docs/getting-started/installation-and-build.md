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

