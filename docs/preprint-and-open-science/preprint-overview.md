### Part 5: Preprint & Open Science Artifacts (`preprint-and-open-science/*`)

This section contains 5 academic research documentation files for `sentinel-lab`: the preprint paper abstract and theoretical findings, the permanent CERN/Zenodo DOI archive specification, local IEEE LaTeX compilation instructions, citation/BibTeX attribution guidelines, and open-access licensing frameworks.

---

### File: `sentinel-lab/docs/preprint-and-open-science/preprint-overview.md`

```markdown
# Academic Preprint: Paper Abstract & Theoretical Formalization

The research testbed and empirical findings in `sentinel-lab` accompany the academic preprint manuscript:  
**"Autonomous Sub-Microsecond Cyber-Physical Threat Mitigation via In-Kernel eBPF/XDP and Heterogeneous Edge AI Acceleration"** (Saberifard et al., 2026).

---

## 1. Manuscript Abstract

```text
================================================================================
                               PAPER ABSTRACT
================================================================================
Modern cyber-physical systems (CPS)—including electrical transmission grids, 
water distribution facilities, and autonomous vehicles—face sophisticated 
adversarial intrusions capable of causing irreversible physical damage in 
milliseconds. Traditional Security Information and Event Management (SIEM) and 
Intrusion Prevention Systems (IPS) fail in these environments due to the "CSV 
illusion" and severe architectural latency: extracting features in user space 
and traversing operating system networking stacks introduces alert-to-mitigation 
delays of 15 seconds to several milliseconds.

In this work, we present Sentinel-Lab, an open, reproducible, hardware-in-the-
loop research testbed for sub-microsecond threat mitigation. We introduce the 
SLAB (Sentinel-Lab) Universal Binary Wire Protocol, a zero-copy format embedding 
continuous multidimensional feature tensors alongside ground-truth labels. 

Operating directly inside Linux network driver space via eBPF/XDP, our testbed 
evaluates incoming flows before kernel socket buffer (sk_buff) allocation occurs. 
Benchmarked against the Canadian Institute for Cybersecurity CIC-IDS-2017 PortScan 
dataset streamed at wire speed (> 60,000 EPS) over raw AF_PACKET sockets, our 
architecture achieves an F1-score of 0.9988 while enforcing in-kernel packet drops 
in under 0.84 microseconds (median p50: 0.72 microseconds). We provide empirical 
evaluations across heterogeneous edge AI silicon—benchmarking Intel Core Ultra NPUs 
against NVIDIA TensorRT GPUs—and release all source code, LaTeX manuscripts, 
and datasets under open-access licensing with permanent CERN/Zenodo DOIs.
================================================================================
```

---

## 2. Theoretical Mathematical Formalization

The manuscript formalizes the mitigation latency gap between traditional user-space systems and driver-level eBPF mitigation:

$$\Delta t_{\text{mitigate}}^{\text{Legacy}} = t_{\text{DMA}} + t_{\text{NAPI}} + t_{\text{alloc\_skb}} + t_{\text{stack}} + t_{\text{context\_switch}} + t_{\text{inference}} + t_{\text{syscall\_drop}}$$

$$\Delta t_{\text{mitigate}}^{\text{Sentinel}} = t_{\text{DMA}} + t_{\text{XDP\_hook}} + t_{\text{bounds\_check}} + t_{\text{bpf\_lookup}} + t_{\text{XDP\_DROP}}$$

Where:
* $\Delta t_{\text{mitigate}}^{\text{Legacy}} \ge 15{,}000\,\text{ns}$ to $50{,}000{,}000\,\text{ns}$ ($15\,\mu\text{s}$ to $50\,\text{ms}$).
* $\Delta t_{\text{mitigate}}^{\text{Sentinel}} \le 840\,\text{ns}$ ($0.84\,\mu\text{s}$ SLA bound).
```

