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

