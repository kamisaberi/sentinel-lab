# Comparative Benchmarks

Empirical comparative studies evaluating `sentinel-lab` against established industry security platforms.

## Section Navigation

- [Benchmark Methodology](./benchmark-methodology.md): Isolated bare-metal testbed hardware specifications and experimental controls.
- [vs. Suricata 7.0](./sentinel-vs-suricata-7.md): Comparing driver-level eBPF/XDP drops against Suricata inline NFQUEUE ($11{,}666\times$ speedup).
- [vs. Snort 3](./sentinel-vs-snort-3.md): Evaluating DAQ `afpacket` mode against native driver XDP at 10GbE wire speed.
- [vs. Elastic SIEM](./sentinel-vs-elastic-siem.md): Measuring the detection-to-mitigation time gap ($0.84\,\mu\text{s}$ vs. $4.2\text{s}$).
- [vs. Splunk Enterprise](./sentinel-vs-splunk-enterprise.md): Resource footprint comparison ($180\text{ MB}$ RAM vs. $68\text{ GB}$ RAM).
- [Latency CDF Percentiles](./latency-cdf-percentiles.md): Empirical Cumulative Distribution Function plots ($p10 - p99.9$).
- [Reproducibility Audit](./reproducibility-audit.md): Independent third-party audit verification attestation (TalTech Cyber Security Lab).
