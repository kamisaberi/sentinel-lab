# Installation & Build

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Compiling eBPF bytecode (build_bpf.sh) and the native C++ testbed.

## Order

Bytecode first, testbed second — the build script enforces it.

## Verify

The smoke suite must pass before any benchmark counts.

```bash
chmod +x bpf/build_bpf.sh
./bpf/build_bpf.sh
mkdir -p build && cd build
cmake .. -DENABLE_OPENVINO=ON -DBUILD_TESTS=ON
make -j$(nproc)
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
