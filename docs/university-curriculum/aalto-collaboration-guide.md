---

### File: `sentinel-lab/docs/university-curriculum/aalto-collaboration-guide.md`

```markdown
# Research Collaboration Guide: Aalto University Secure Systems Group

This guide outlines collaborative research initiatives with the **Secure Systems Group at Aalto University (Finland)**, focusing on trusted hardware, Linux kernel security, and verified platform roots of trust.

---

## 1. Institutional Research Alignment

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ Aalto University: Secure Systems Group                      │
 ├─────────────────────────────────────────────────────────────┤
 │ • Primary Focus: Trusted Execution Environments (TEEs),     │
 │   Platform Security, Linux Kernel Hardening                 │
 │ • Key Competencies: Hardware Attestation, Formal Proofs     │
 └──────────────────────────────┬──────────────────────────────┘
                                │
        ┌───────────────────────┴───────────────────────┐
        ▼ Collaborative Track 1                         ▼ Collaborative Track 2
 ┌─────────────────────────────┐         ┌─────────────────────────────┐
 │ Formal BPF Verification     │         │ TPM 2.0 PCR Quoting Models  │
 │ Mathematically proving      │         │ Verifying remote attestation│
 │ bounds safety and cycle     │         │ protocols under adversarial │
 │ determinism for XDP filters.│         │ hypervisor conditions.      │
 └─────────────────────────────┘         └─────────────────────────────┘
```

---

## 2. Collaborative Research Themes

1. **Formal Proofs of XDP Memory Boundaries:** Utilizing formal verification tools (e.g., SeaHorn, Coq) to prove that `xdp_threat_filter()` cannot trigger out-of-bounds pointer dereferences across arbitrary packet encapsulations.
2. **Confidential Computing at the Edge:** Exploring hardware attestation extensions utilizing Intel TDX (Trust Domain Extensions) and AMD SEV-SNP to protect model weights during runtime execution.
```

---

### File: `sentinel-lab/docs/university-curriculum/student-grant-support.md`

```markdown
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
```

