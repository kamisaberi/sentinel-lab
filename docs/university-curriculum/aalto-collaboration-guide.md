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

