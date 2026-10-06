# Hardware-in-the-Loop (HIL) Testbed Design

`sentinel-lab` bridges theoretical algorithms and physical network hardware using a **Hardware-in-the-Loop (HIL)** architecture.

---

## 1. HIL Testbed Configuration

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ TRAFFIC GENERATOR / PACKET INJECTOR (Node A)                │
 │  - MoonGen / Python SLAB Raw Socket Blaster                 │
 │  - Replays CIC-IDS-2017 PortScan traffic at 60k+ EPS        │
 └──────────────────────────────┬──────────────────────────────┘
                                │ Physical 10GbE Fiber (DAC) SFP+
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ DEVICE UNDER TEST: SENTINEL-LAB (Node B)                    │
 │                                                             │
 │  ┌───────────────────────────────────────────────────────┐  │
 │  │ Linux Kernel Driver: Native eBPF/XDP Hook             │  │
 │  │ -> Drops malicious IPs before socket allocation       │  │
 │  └───────────────────────────┬───────────────────────────┘  │
 │                              │ Raw SLAB Frame Transfer      │
 │                              ▼                              │
 │  ┌───────────────────────────────────────────────────────┐  │
 │  │ Edge AI Silicon: Intel Core Ultra NPU / NVIDIA GPU    │  │
 │  │ -> Evaluates 32-dim flow vector in microsecond SLA    │  │
 │  └───────────────────────────────────────────────────────┘  │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Invariants of the HIL Methodology

* **Live Line-Rate Ingestion:** Traffic is transmitted over physical network interface cards (e.g., Intel X520 10GbE or E810 25GbE) rather than software mock queues.
* **Driver-Level Packet Drops:** Verified by observing the physical hardware drop counters (`ethtool -S eth0 | grep rx_dropped`) on the network controller.
* **Silicon Isolation:** AI models run directly on physical acceleration coprocessors (Intel Neural Processing Units, NVIDIA Tensor Cores) subjected to real-world PCIe bus transfers.

