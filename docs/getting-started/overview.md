---

### File: `sentinel-lab/docs/getting-started/overview.md`

```markdown
# Open Research Testbed: Sub-Microsecond Threat Mitigation

Intrusion detection research in academic literature suffers from widespread methodological flaws:
* **The "CSV Illusion":** Over $85\%$ of published papers train classifiers on static CSV rows using Pandas and Scikit-Learn, reporting high theoretical accuracies ($> 99\%$) while ignoring feature extraction delays, socket buffer overheads, and network jitter.
* **The Mitigation Void:** Traditional research evaluates *detection* (alerting) rather than *mitigation* (active containment). In operational networks, an alert emitted $30\text{ seconds}$ late fails to stop catastrophic physical failure.
* **Hardware Disconnection:** Models are rarely benchmarked on resource-constrained edge NPUs or integrated embedded silicon under realistic thermal budgets.

`sentinel-lab` provides the academic community with an **open, reproducible, hardware-in-the-loop research harness**.

---

## 1. Research Contributions

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. The SLAB Binary Wire Protocol                            │
 │    A self-describing, zero-copy packet wire format that     │
 │    embeds ground-truth labels alongside raw feature tensors.│
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ 2. End-to-End Latency Measurement Physics                   │
 │    Nanosecond-level profiling from physical wire arrival to │
 │    in-kernel eBPF packet mitigation (< 0.84 µs).            │
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ 3. Automated Benchmark Pipeline                             │
 │    Downloads official CIC-IDS-2017 datasets, serializes to  │
 │    SLAB, blasts over raw sockets, and outputs LaTeX tables. │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Supported Research Benchmarks

* **CIC-IDS-2017 (Canadian Institute for Cybersecurity):** PortScan, DoS, and BruteForce subsets.
* **UNSW-NB15 (University of New South Wales):** Advanced lateral movement and modern exploit vectors.
* **Industrial SCADA Traces:** Triton, Stuxnet, and Industroyer2 cyber-physical attack replays.
```

