---

### File: `sentinel-lab/docs/slab-protocol/multi-dataset-compatibility.md`

```markdown
# Multi-Dataset Cross-Compatibility

Because the SLAB protocol is self-describing through its `dimensions` field ($D$), a single `sentinel_lab` deployment can evaluate disparate intrusion detection benchmarks without recompiling the testbed engine.

---

## 1. Supported Academic Dataset Mappings

| Benchmark Corpus | Originating University | Raw Dimension ($D$) | SLAB Wire Size | Primary Exploit Vectors |
| :--- | :--- | :--- | :--- | :--- |
| **CIC-IDS-2017** | Univ. of New Brunswick (CIC) | **$32$** Features | $152\text{ Bytes}$ | PortScan, DoS, BruteForce |
| **UNSW-NB15** | Australian Cyber Security Centre | **$42$** Features | $192\text{ Bytes}$ | Fuzzers, Backdoors, Worms |
| **NSL-KDD** | University of New Brunswick | **$41$** Features | $188\text{ Bytes}$ | Legacy SYN Floods, R2L, U2R |
| **SCADA Triton** | Industrial Testbed (OT) | **$16$** Features | $88\text{ Bytes}$ | TriStation Safety Overrides |
| **CIC-DDoS-2019**| Univ. of New Brunswick (CIC) | **$80$** Features | $344\text{ Bytes}$ | Volumetric NTP/DNS Amplification |

---

## 2. Dynamic Dimension Handshake

When `sentinel_lab` parses a packet, it reads `header->dimensions`:
* If the incoming frame dimension matches the loaded AI model's input shape, evaluation executes directly.
* If a dimension mismatch occurs (e.g. an $80\text{-dim}$ frame arrives at a $32\text{-dim}$ model), the packet is skipped and logged to prevent memory over-runs.
```

