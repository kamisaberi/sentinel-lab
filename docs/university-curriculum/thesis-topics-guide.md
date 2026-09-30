### Part 8: University Curriculum & Graduate Thesis Integration (`university-curriculum/*`)

This section contains 5 academic curriculum and graduate research integration guides: ready-made Master's and PhD thesis proposals, course laboratory syllabi for Advanced OS and Network Security, institutional collaboration frameworks for **TalTech** and **Aalto University**, and guidelines for leveraging `sentinel-lab` in academic grant applications.

---

### File: `sentinel-lab/docs/university-curriculum/thesis-topics-guide.md`

```markdown
# Graduate Thesis Proposals: Low-Latency Systems & Edge AI

`sentinel-lab` provides a turn-key, reproducible testbed for Master of Science (MSc) and Doctor of Philosophy (PhD) students conducting research in computer systems, operating systems, network security, and cyber-physical systems.

Below are four ready-made, high-impact research proposals pre-aligned with `sentinel-lab`'s architectural primitives.

---

## Proposal 1: eBPF CO-RE Compilation Across Heterogeneous Edge Silicon

* **Degree Level:** Master of Science (MSc)
* **Primary Tracks:** Operating Systems, Computer Architecture, Embedded Linux
* **Problem Statement:** Standard eBPF programs rely on target-specific Linux kernel headers (`linux-headers-$(uname -r)`), limiting runtime portability across diverse edge architectures (x86_64, ARM64, and RISC-V). Compile Once – Run Everywhere (CO-RE) with BPF Type Format (`vmlinux.h`) promises portability, but introduces verifier instruction boundary challenges across heterogeneous memory layouts.
* **Research Objective:** Design, implement, and benchmark an automated eBPF CO-RE compilation pipeline within `sentinel-lab` that achieves $< 0.84\,\mu\text{s}$ mitigation parity across Intel Core Ultra, Rockchip RK3588, and Raspberry Pi 5 hardware.
* **Key Research Artifacts:** Extended `bpf/xdp_filter.c` with BTF relocations, automated cross-architecture verification scripts, and empirical jitter benchmarks.

---

## Proposal 2: Microsecond Residual Explainability (MRD) vs. Permutation Attribution

* **Degree Level:** Master of Science (MSc) or PhD
* **Primary Tracks:** Explainable AI (XAI), Critical Infrastructure Defense
* **Problem Statement:** Post-hoc feature attribution methods (such as SHAP and LIME) require thousands of model perturbations, taking hundreds of milliseconds to several seconds to compute. In high-frequency network environments, these delays prevent operators from understanding root-cause anomalies before automated mitigation actions occur.
* **Research Objective:** Formalize the mathematical convergence of Microsecond Residual Decomposition (MRD) against Shapley values on tabular NetFlow data. Prove conditions under which autoencoder reconstruction residuals provide equivalent top-3 feature attributions in $< 80\,\text{nanoseconds}$.
* **Key Research Artifacts:** Comparative attribution accuracy theorems, mathematical proofs of residual convergence, and real-time visualization widgets for the Web Command Center.

---

## Proposal 3: Hardware-Enforced Anti-Cloning Protocols for Virtualized Edge Appliances

* **Degree Level:** PhD Dissertation Chapter
* **Primary Tracks:** Hardware Security, Cryptography, Virtualization
* **Problem Statement:** Virtual security appliances deployed in cloud environments can be duplicated via hypervisor snapshots. Software tokens and disk certificates are cloned alongside the machine image, enabling unauthorized clone instances to bypass fleet licensing and poison collective defense networks.
* **Research Objective:** Develop a formal cryptographic challenge-response protocol anchoring virtual appliance identity to physical TPM 2.0 PCR quotes and monotonic hardware counter sequences. Prove security against rollback attacks and concurrent clone executions.
* **Key Research Artifacts:** Cryptographic protocol proofs in ProVerif/Tamarin, TSS2 C++ driver extensions, and an empirical evaluation harness simulating VMware snapshot replication.

---

## Proposal 4: Continual Representation Learning on Non-Stationary SCADA Telemetry

* **Degree Level:** PhD Dissertation
* **Primary Tracks:** Machine Learning, Cyber-Physical Systems (CPS)
* **Problem Statement:** Cyber-physical systems experience continuous statistical drift due to environmental temperature shifts, mechanical wear, and grid load rebalancing. Unsupervised models retrained on ambient traffic are vulnerable to adversarial "boiling-the-frog" data poisoning.
* **Research Objective:** Formulate a constrained continual optimization objective combining Masked Autoencoders (MAE) with an invariant topological safety projection. Prove that logical conjunction gates preserve historical exploit detection ($S(\theta^*) = 1.000$) while minimizing reconstruction loss on non-stationary ambient baselines.
* **Key Research Artifacts:** Mathematical formulations of safety-constrained empirical risk minimization (ERM), empirical evaluations on the 6-month drift dataset, and publication in IEEE TDSC or ACM CCS.
```

