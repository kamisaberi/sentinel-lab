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

