# Compiling the LaTeX Paper

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Compiling paper.tex locally (IEEE single-column pdflatex guide).

## Build

pdflatex + bibtex, twice; figures regenerate from CSVs.

## Check

Page count, font embedding, and hyperlink integrity before submit.

```bash
$ cd paper && pdflatex paper.tex && bibtex paper && pdflatex paper.tex
$ ls -l paper.pdf
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
