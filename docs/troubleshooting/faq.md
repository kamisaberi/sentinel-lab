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

