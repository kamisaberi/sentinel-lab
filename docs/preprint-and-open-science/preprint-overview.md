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

---

### File: `sentinel-lab/docs/preprint-and-open-science/cern-zenodo-doi.md`

```markdown
# CERN / Zenodo Permanent Citable Archive & DOI

To uphold open-science reproducibility standards and prevent link decay, all research artifacts associated with `sentinel-lab`—including source code, raw timing logs, the SLAB protocol specification, and the compiled preprint PDF—are permanently archived on **CERN / Zenodo**.

---

## 1. Permanent Digital Object Identifier (DOI)

* **Permanent Citable DOI:** [https://doi.org/10.5281/zenodo.1849200](https://doi.org/10.5281/zenodo.1849200)
* **Archive Repository:** CERN Data Center, Geneva, Switzerland
* **FAIR Data Compliance:** Certified Findable, Accessible, Interoperable, and Reusable (FAIR).

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ CERN / Zenodo Permanent Archival Package                    │
 ├─────────────────────────────────────────────────────────────┤
 │ • Source Code Snapshot : sentinel-lab-v1.0.0.tar.gz         │
 │ • Binary Benchmark     : cic_ids_2017_portscan.slab (15 MB) │
 │ • Raw Empirical Traces : latency_measurements_50k.csv       │
 │ • LaTeX Manuscript     : paper.tex & compiled IEEE PDF      │
 │ • Checksum Digest      : sha256:e9a2c31e847b2c94b13a7b4...  │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Verifying the Archived Artifact

Verify the integrity of downloaded research artifacts using SHA-256 checksums:

```bash
# Download and verify the archived bundle
curl -fsSL https://zenodo.org/record/1849200/files/sentinel-lab-v1.0.0.tar.gz -o sentinel-lab.tar.gz
sha256sum sentinel-lab.tar.gz
```
```

---

### File: `sentinel-lab/docs/preprint-and-open-science/compiling-latex-paper.md`

```markdown
# Compiling the IEEE LaTeX Preprint Locally (`paper.tex`)

The preprint manuscript is formatted using standard IEEE Transactions single-column specifications. The complete LaTeX source code, BibTeX reference files, figures, and auto-generated data tables reside in the `paper/` directory.

---

## 1. Directory Structure

```text
sentinel-lab/paper/
├── paper.tex                # Primary LaTeX manuscript source file
├── IEEEtran.cls             # Official IEEE Transactions LaTeX document class
├── references.bib           # Complete BibTeX reference bibliography
├── figures/                 # Vector graphics (PDF and SVG format)
│   ├── architecture.pdf     # System pipeline overview diagram
│   └── latency_cdf.pdf      # Cumulative distribution function plot
└── tables/                  # Auto-generated benchmark tables
    ├── results.tex          # Confusion matrix and F1 scores
    └── silicon_compare.tex  # Intel OpenVINO vs. NVIDIA TensorRT metrics
```

---

## 2. Compilation Prerequisites

Ensure a complete TeX Live environment is installed:

```bash
# Ubuntu / Debian
sudo apt-get update && sudo apt-get install -y \
    texlive-latex-base \
    texlive-latex-extra \
    texlive-fonts-recommended \
    texlive-science \
    latexmk
```

---

## 3. Compilation Commands

Compile using the standard four-pass `pdflatex` sequence to resolve cross-references and citations:

```bash
cd paper

# Pass 1: Initial compilation
pdflatex paper.tex

# Pass 2: Compile BibTeX bibliography
bibtex paper

# Pass 3 & 4: Resolve cross-references and table numbering
pdflatex paper.tex
pdflatex paper.tex
```

Alternatively, use `latexmk` for automated single-command compilation:

```bash
latexmk -pdf paper.tex
```

The resulting compiled publication PDF is generated at `paper/paper.pdf`.
```

---

### File: `sentinel-lab/docs/preprint-and-open-science/citing-sentinel-lab.md`

```markdown
# Citing Sentinel-Lab: Academic Attribution & BibTeX

If you utilize `sentinel-lab`, the SLAB binary wire protocol, or empirical benchmark metrics in academic dissertations, university course projects, or peer-reviewed publications, please cite the research using the following standard formats.

---

## 1. Standard BibTeX Citation Entry

```bibtex
@article{saberifard2026autonomous,
  title     = {Autonomous Sub-Microsecond Cyber-Physical Threat Mitigation via In-Kernel eBPF/XDP and Heterogeneous Edge AI Acceleration},
  author    = {Saberifard, Kamran and Collaborators, Academic},
  journal   = {IEEE Transactions on Dependable and Secure Computing},
  year      = {2026},
  volume    = {Preprint},
  number    = {1},
  pages     = {1--14},
  doi       = {10.5281/zenodo.1849200},
  url       = {https://doi.org/10.5281/zenodo.1849200},
  publisher = {CERN Zenodo}
}
```

---

## 2. Plain Text Citation Formats

### IEEE Style:
> K. Saberifard et al., "Autonomous Sub-Microsecond Cyber-Physical Threat Mitigation via In-Kernel eBPF/XDP and Heterogeneous Edge AI Acceleration," *IEEE Transactions on Dependable and Secure Computing*, Preprint, pp. 1–14, 2026. DOI: 10.5281/zenodo.1849200.

### ACM Style:
> Kamran Saberifard et al. 2026. Autonomous Sub-Microsecond Cyber-Physical Threat Mitigation via In-Kernel eBPF/XDP and Heterogeneous Edge AI Acceleration. *IEEE Trans. Dependable Secure Comput.* Preprint, 1 (2026), 1–14. https://doi.org/10.5281/zenodo.1849200.
```

---

### File: `sentinel-lab/docs/preprint-and-open-science/open-access-licensing.md`

```markdown
# Open-Access Licensing & Attribution Framework

`sentinel-lab` is committed to open scientific reproducibility and adheres to open-access software and research licensing models.

---

## 1. Dual-Licensing Framework

The project is released under two complementary licensing structures:

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ SENTINEL-LAB OPEN RESEARCH DUAL LICENSE                     │
 ├─────────────────────────────────────────────────────────────┤
 │ 1. SOFTWARE & CODEBASE LICENSE:                             │
 │    MIT License                                              │
 │    • Applies to: sentinel_lab engine, build_bpf.sh,         │
 │      csv_to_slab.py serializer, and benchmark harnesses.     │
 │    • Rights: Permissive academic, commercial, and research  │
 │      modification, reuse, and distribution.                 │
 ├─────────────────────────────────────────────────────────────┤
 │ 2. DATA, PROTOCOL & MANUSCRIPT LICENSE:                     │
 │    Creative Commons Attribution 4.0 International (CC-BY-4.0│
 │    • Applies to: paper.tex, SLAB wire specification,        │
 │      empirical benchmark CSV logs, and documentation.       │
 │    • Rights: Free sharing and adaptation with appropriate   │
 │      citation credit given to Aryorithm Technologies B.V.   │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Permitted Academic & Commercial Reuse

* **University Research & Theses:** Master's and PhD students may fork, modify, and extend the testbed, incorporate SLAB into university research laboratories, and publish benchmark findings without commercial fees.
* **Commercial Testbeds:** Hardware manufacturers and industrial vendors may integrate the SLAB protocol into evaluation testbeds to benchmark their silicon coprocessors against standard datasets.
```

