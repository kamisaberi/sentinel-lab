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

