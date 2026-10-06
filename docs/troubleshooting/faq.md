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

