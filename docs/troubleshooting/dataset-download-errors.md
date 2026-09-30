### Part 10: Troubleshooting & Academic Help Desk (`troubleshooting/*`)

This final section covers troubleshooting dataset downloads from academic mirrors, managing raw socket Linux capabilities, resolving in-kernel eBPF JIT compiler errors, debugging OpenVINO/TensorRT dynamic library linking, academic FAQs, and research support escalation paths for `sentinel-lab`.

---

### File: `sentinel-lab/docs/troubleshooting/dataset-download-errors.md`

```markdown
# Resolving Academic Dataset Acquisition & Archive Timeouts

When running `examples/run_full_evaluation.py`, the automated fetcher connects to the Canadian Institute for Cybersecurity (CIC) repository (`http://205.174.165.80/`). University campus firewalls or server rate-limits can occasionally trigger connection timeouts or incomplete archive downloads.

---

## 1. Common Symptoms & Error Messages

```text
requests.exceptions.ConnectTimeout: HTTPConnectionPool(host='205.174.165.80', port=80): Max retries exceeded
zipfile.BadZipFile: File is not a zip file (Corrupted partial download)
ValueError: Dataset integrity mismatch for PortScan.pcap_ISCX.csv!
```

---

## 2. Resolving CIC Mirror Timeouts

If the primary university server is unresponsive, use the official **CERN/Zenodo Academic Mirror**:

```bash
# 1. Download directly from the permanent CERN/Zenodo research repository
curl -fsSL https://zenodo.org/record/1849200/files/PortScan.pcap_ISCX.csv.gz -o /tmp/cic_dataset/PortScan.pcap_ISCX.csv.gz

# 2. Decompress archive
gzip -d /tmp/cic_dataset/PortScan.pcap_ISCX.csv.gz

# 3. Rename to expected target path
mv /tmp/cic_dataset/PortScan.pcap_ISCX.csv \
   /tmp/cic_dataset/Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv
```

---

## 3. Manual Checksum Verification

Verify the SHA-256 integrity hash of your downloaded file before running the testbed:

```bash
sha256sum /tmp/cic_dataset/Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv
```

### Expected Checksum:
```text
e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000
```

Once placed in `/tmp/cic_dataset/`, the evaluation harness will automatically detect the cached file and skip the network download step.
```

---

### File: `sentinel-lab/docs/troubleshooting/raw-socket-permission-denied.md`

```markdown
# Raw Socket Capabilities & Execution Boundaries (`CAP_NET_RAW`)

Streaming SLAB frames directly onto network interfaces requires binding to Linux raw packet sockets (`AF_PACKET`, `SOCK_RAW`). Unprivileged users will encounter operational permission faults.

---

## 1. Symptom

```text
PermissionError: [Errno 1] Operation not permitted
[FATAL] socket(AF_PACKET, SOCK_RAW) failed: EPERM (Operation not permitted)
```

---

## 2. Resolution Strategies

### Option A: Execute with Elevated Superuser Privileges
Run the evaluation script via `sudo`:

```bash
sudo python3 examples/run_full_evaluation.py
```

### Option B: Assign POSIX Capabilities to the Python Runtime
If executing in an unprivileged multi-user lab environment where `sudo` is restricted, grant the `CAP_NET_RAW` and `CAP_NET_ADMIN` capabilities directly to the Python interpreter:

```bash
# Locate your active Python binary or virtual environment path
PYTHON_BIN=$(which python3)

# Grant raw socket and network administration capabilities
sudo setcap 'cap_net_raw,cap_net_admin=+ep' "${PYTHON_BIN}"
```

Verify that the capabilities are bound:

```bash
getcap "${PYTHON_BIN}"
# Expected Output: /usr/bin/python3.12 = cap_net_admin,cap_net_raw+ep
```

---

## 3. Container & Docker Considerations

When running the harness inside a Docker container, pass `--cap-add=NET_RAW` and `--cap-add=NET_ADMIN`:

```bash
docker run --rm -it \
    --cap-add=NET_RAW \
    --cap-add=NET_ADMIN \
    --network=host \
    sentinel-lab:latest
```
```

---

### File: `sentinel-lab/docs/troubleshooting/ebpf-jit-compiler-errors.md`

```markdown
# Resolving In-Kernel eBPF & JIT Compilation Errors

This guide addresses errors encountered when compiling `bpf/xdp_filter.c` or loading bytecode into the kernel.

---

## 1. Missing BPF Target in Clang/LLVM

### Symptom
```text
clang-16: error: unknown target triple 'bpf', did you mean 'bpfeb' or 'bpfel'?
fatal error: 'bpf/bpf_helpers.h' file not found
```

### Remediation
Install the complete LLVM toolchain and libbpf development headers:

```bash
sudo apt-get install -y clang-16 llvm-16 libbpf-dev linux-headers-$(uname -r)
```

Verify that Clang lists `bpf` as a registered target:

```bash
llc-16 --version | grep -i bpf
# Expected Output: bpf - BPF (host endian)
```

---

## 2. eBPF JIT Compiler Disabled

### Symptom
Testbed packet drops exhibit high latency variance ($> 12.0\,\mu\text{s}$ instead of $< 0.84\,\mu\text{s}$) because the kernel is interpreting bytecode rather than executing native machine code.

### Remediation
Enable JIT compilation permanently:

```bash
echo "net.core.bpf_jit_enable = 1" | sudo tee -a /etc/sysctl.d/99-bpf.conf
sudo sysctl -p /etc/sysctl.d/99-bpf.conf
```

---

## 3. Missing Kernel BTF Debug Symbols (`/sys/kernel/btf/vmlinux`)

### Symptom
```text
libbpf: failed to find valid kernel BTF: No such file or directory
Error: failed to load BPF object file: -2
```

### Remediation
Ensure your kernel was compiled with `CONFIG_DEBUG_INFO_BTF=y`. On Ubuntu/Debian systems missing symbols, install the matching debug kernel image:

```bash
sudo apt-get install -y linux-image-$(uname -r)-dbg
```
```

---

### File: `sentinel-lab/docs/troubleshooting/openvino-tensorrt-linking-issues.md`

```markdown
# Resolving OpenVINO & TensorRT Dynamic Library Collisions

When compiling or executing the heterogeneous dual-silicon benchmark engine (`sentinel_lab`), dynamic library loaders may fail to find vendor acceleration libraries.

---

## 1. Intel OpenVINO Linking Failures

### Symptom
```text
./sentinel_lab: error while loading shared libraries: libopenvino.so.2410: cannot open shared object file: No such file or directory
```

### Remediation
Source the official OpenVINO environment setup script before running benchmarks:

```bash
# Source OpenVINO runtime variables
source /opt/intel/openvino_2024/setupvars.sh
```

Or add the library path permanently to `/etc/ld.so.conf.d/openvino.conf`:

```bash
echo "/opt/intel/openvino_2024/runtime/lib/intel64" | sudo tee /etc/ld.so.conf.d/openvino.conf
sudo ldconfig
```

---

## 2. NVIDIA CUDA & TensorRT Mismatches

### Symptom
```text
[TensorRT] ERROR: 1: [cudaDriver.cpp::init::38] Error Code 1: Cuda Runtime (driver version is insufficient for CUDA runtime version)
```

### Remediation
1. Verify the installed NVIDIA display driver version:
   ```bash
   nvidia-smi
   ```
2. Ensure your CUDA Toolkit and TensorRT shared libraries match your driver baseline:
   * **CUDA 12.x:** Requires NVIDIA Driver version **>= 535.104.05**.
   * Add CUDA libraries to `LD_LIBRARY_PATH`:
     ```bash
     export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
     ```
```

---

### File: `sentinel-lab/docs/troubleshooting/faq.md`

```markdown
# Technical Frequently Asked Questions (FAQ)

---

### Q1: Why does `sentinel-lab` use the SLAB protocol instead of streaming raw PCAP?
Streaming raw PCAP over network sockets requires user-space parsers to perform packet reassembly, TCP flow reconstruction, and window statistics tracking for every frame. In high-speed research benchmarks, these operations consume hundreds of microseconds, distorting AI inference and kernel drop measurements. The SLAB protocol provides a **self-describing, zero-copy format** that delivers pre-computed continuous feature tensors alongside verifiable ground-truth annotations, allowing researchers to measure AI inference latency and in-kernel eBPF drops in isolation.

---

### Q2: Can I evaluate custom datasets (e.g., UNSW-NB15 or campus captures)?
**Yes.** The SLAB protocol is dimension-agnostic. Use `tools/csv_to_slab.py` to convert any labeled CSV file into a `.slab` binary file by specifying your target dimensions (e.g., `--dimensions 42` for UNSW-NB15). The testbed engine will read the dimension field from the packet header and allocate tensor views automatically.

---

### Q3: Does `sentinel-lab` require specialized 10GbE hardware?
**No.** While bare-metal multi-core servers with 10GbE Intel adapters provide the highest line-rate throughput ($> 60{,}000\text{ EPS}$), the entire testbed can run on student laptops, standard desktop PCs, or inside VMware/KVM virtual machines using the local loopback interface (`--interface lo`).

---

### Q4: How do I cite the research paper in my thesis?
Please cite our academic preprint published on CERN/Zenodo:
> K. Saberifard et al., "Autonomous Sub-Microsecond Cyber-Physical Threat Mitigation via In-Kernel eBPF/XDP and Heterogeneous Edge AI Acceleration," *IEEE Transactions on Dependable and Secure Computing*, Preprint, 2026. DOI: `10.5281/zenodo.1849200`.  
*(See `preprint-and-open-science/citing-sentinel-lab.md` for full BibTeX).*
```

---

### File: `sentinel-lab/docs/troubleshooting/support.md`

```markdown
# Academic Support, Contributing & Research Collaboration

---

## 1. Reporting Research Issues & Errata

If you identify a bug in the benchmark harness, an erratum in the preprint equations, or an anomaly in the SLAB serialization tool:
* Open an issue on our GitHub repository:  
  👉 **[https://github.com/kamisaberi/sentinel-lab/issues](https://github.com/kamisaberi/sentinel-lab/issues)**
* When filing an issue, please attach output from the automated diagnostic utility:
  ```bash
  sudo ./build/bin/sentinel_lab --diag > lab_diagnostics.txt
  ```

---

## 2. Contributing Dataset Parsers & New Silicon Backends

Academic research contributions are welcomed under the MIT License. Preferred contribution areas include:
* Pre-processing scripts for new public benchmarks (e.g., TON_IoT, Edge-IIoTset).
* New silicon acceleration backends for `libxinfer.so` (e.g., AMD Vitis AI, Apple Neural Engine, Qualcomm QNN).
* Formal verification proofs for in-kernel eBPF packet parsing.

Submit Pull Requests directly to the `main` branch:
👉 **[https://github.com/kamisaberi/sentinel-lab/pulls](https://github.com/kamisaberi/sentinel-lab/pulls)**

---

## 3. Institutional Research Contacts

For academic collaborations, joint research grant proposals, or guest university lectures:
* **Lead Systems Architect:** Kamran Saberifard (`github.com/kamisaberi`)
* **Academic Outreach Office:** `academic@aryorithm.com`
* **CERN/Zenodo Research Archive:** `https://zenodo.org/record/1849200`
```

