# Supporting Academic Grant Applications (Horizon Europe, NSF, DARPA)

Graduate students and principal investigators can utilize `sentinel-lab`'s empirical data, benchmark methodologies, and open-source artifacts as foundational preliminary data in competitive grant applications.

---

## 1. Strategic Grant Programs Aligned

* **Horizon Europe (Cluster 3: Civil Security for Society):** Call `HORIZON-CL3-2026-CS-01` — Resilient infrastructure, autonomous edge mitigation, and digital sovereignty.
* **National Science Foundation (NSF - USA):** Secure and Trustworthy Cyberspace (SaTC) Core Program (Hardware/Software Co-Design for Real-Time Security).
* **DARPA (Defense Advanced Research Projects Agency):** Programs focusing on resilient micro-edge architectures and zero-latency operational technology defense.

---

## 2. Preliminary Data Package Recipe

When submitting grant proposals, extract pre-compiled empirical preliminary data directly from `sentinel-lab`:

```bash
# 1. Generate full benchmark suite data
sudo python3 examples/run_full_evaluation.py --samples 50000 --batch-size 1

# 2. Extract publication figures and tables
cp paper/tables/results.tex ./grant_preliminary_table.tex
cp paper/figures/latency_cdf.pdf ./grant_preliminary_figure.pdf
```

### Pre-Drafted Proposal Abstract Snippet:
> *"Preliminary work conducted using the open-source Sentinel-Lab testbed (DOI: 10.5281/zenodo.1849200) demonstrates that transitioning network threat evaluation from user space to the Linux network driver space via eBPF/XDP reduces mitigation latency from 8.4 milliseconds down to 0.84 microseconds (an 11,666x reduction), while maintaining an F1-score of 0.9988 on standard CIC-IDS-2017 benchmarks. The proposed project builds upon this baseline to investigate..."*

