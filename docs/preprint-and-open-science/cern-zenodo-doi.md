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

