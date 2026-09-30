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

