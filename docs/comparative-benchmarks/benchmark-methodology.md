### Part 7: Comparative Empirical Benchmarks (`comparative-benchmarks/*`)

This section contains 7 empirical benchmark studies and comparative architectural analyses evaluating `sentinel-lab` against traditional intrusion detection and SIEM platforms: the bare-metal testbed specifications, head-to-head comparisons against **Suricata 7**, **Snort 3**, **Elastic SIEM**, and **Splunk Enterprise**, cumulative latency distribution functions (CDF), and third-party reproducibility audit records.

---

### File: `sentinel-lab/docs/comparative-benchmarks/benchmark-methodology.md`

```markdown
# Bare-Metal Benchmark Testbed Specifications

To ensure scientific validity under peer-review standards, all comparative benchmarks documented in this section were conducted on an isolated bare-metal testbed adhering to standardized hardware and network topologies.

---

## 1. Testbed Hardware Specifications

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ WORKSTATION & SUT (System Under Test)                       │
 ├─────────────────────────────────────────────────────────────┤
 │ • Motherboard : ASUS Pro WS W680-ACE IPMI (PCIe Gen 5)      │
 │ • CPU         : Intel Core i9-14900K (24 Cores, 32 Threads, │
 │                 3.20 GHz Base, 6.0 GHz Turbo)               │
 │ • RAM         : 192 GB (4x 48GB) DDR5-5200 ECC Unbuffered   │
 │ • Storage     : 2x 2TB Samsung 990 PRO PCIe 4.0 NVMe SSD    │
 │ • NIC         : Intel 82599ES (Dual-Port 10GbE SFP+ PCI-e)  │
 │ • OS          : Ubuntu 24.04 LTS (Kernel 6.8.0-31-generic)  │
 └──────────────────────────────▲──────────────────────────────┘
                                │ 10GbE Direct Attach Copper (DAC)
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ TRAFFIC GENERATOR HARNESS (MoonGen / TRex Blaster)          │
 ├─────────────────────────────────────────────────────────────┤
 │ • NIC         : Intel 82599ES 10GbE Dual-Port SFP+          │
 │ • Injection   : Wire-speed raw AF_PACKET SLAB frames        │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Experimental Controls

* **CPU Core Shielding:** Core 2 and Core 3 were completely isolated from the Linux OS task scheduler via `isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3`.
* **Dynamic Frequency Pinning:** Intel SpeedStep and Turbo Boost were locked to a constant frequency using `cpupower frequency-set -g performance`.
* **Dataset Standardization:** All systems evaluated the identical 50,000-flow slice of the Canadian Institute for Cybersecurity **CIC-IDS-2017 PortScan dataset**.
```

