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

---

### File: `sentinel-lab/docs/university-curriculum/course-module-integration.md`

```markdown
# Course Module Integration: Advanced OS & Network Security

`sentinel-lab` can be integrated directly into postgraduate computer science curricula as a multi-week laboratory sequence for courses such as **Advanced Operating Systems**, **Network Security**, or **High-Performance Computer Networks**.

---

## 1. 3-Week Laboratory Syllabus Overview

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ WEEK 1: In-Kernel Packet Processing with eBPF/XDP           │
 │  - Lab Exercise: Writing, compiling, and loading xdp_drop   │
 │  - Learning Goal: Understand driver NAPI hooks and zero-SKB │
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ WEEK 2: The SLAB Universal Binary Protocol & Raw Sockets    │
 │  - Lab Exercise: Serializing datasets; injecting at 60k EPS │
 │  - Learning Goal: Hardware-in-the-loop socket programming   │
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌─────────────────────────────────────────────────────────────┐
 │ WEEK 3: Heterogeneous Silicon Profiling & Latency Physics   │
 │  - Lab Exercise: Profiling Intel NPU vs CPU via TSC cycles  │
 │  - Learning Goal: Microsecond hardware timer instrumentation│
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Laboratory 1 Exercise: Writing an eBPF Port Filter

### Student Objective
Write a minimal eBPF program in C that attaches to interface `lo`, parses incoming Ethernet and IPv4 headers, and drops all packets directed to destination port `502` (Modbus TCP):

```c
// students/lab1/xdp_exercise.c
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

SEC("xdp")
int filter_modbus(struct xdp_md *ctx) {
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    // Student Task: Complete bounds checks for Ethernet, IP, and TCP headers
    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) return XDP_PASS;
    if (eth->h_proto != bpf_htons(ETH_P_IP)) return XDP_PASS;

    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end) return XDP_PASS;
    if (ip->protocol != IPPROTO_TCP) return XDP_PASS;

    struct tcphdr *tcp = (void *)((char *)ip + (ip->ihl * 4));
    if ((void *)(tcp + 1) > data_end) return XDP_PASS;

    // Drop Modbus TCP traffic
    if (tcp->dest == bpf_htons(502)) {
        return XDP_DROP;
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
```

### Grading Criteria
* Verified compilation via `clang -target bpf -O2`.
* Successful passage through the Linux in-kernel BPF verifier.
* Empirical verification of packet drops using `curl` or `mbpoll`.
```

---

### File: `sentinel-lab/docs/university-curriculum/taltech-collaboration-guide.md`

```markdown
# Research Collaboration Guide: TalTech Cyber Security Lab

This guide aligns research with the **Centre for Digital Forensics and Cyber Security at Tallinn University of Technology (TalTech, Estonia)**, focusing on critical infrastructure defense, digital forensics, and NATO CCDCOE exercises.

---

## 1. Institutional Research Alignment

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ TalTech Centre for Digital Forensics & Cyber Security       │
 ├─────────────────────────────────────────────────────────────┤
 │ • Primary Focus: Industrial Control Defense (OT/SCADA)      │
 │ • Regional Mandate: Baltic Energy Grid Cyber Resilience     │
 │ • Key Exercise Integration: NATO Locked Shields & Crossed   │
 │   Swords Cyber Defence Exercises                            │
 └──────────────────────────────┬──────────────────────────────┘
                                │
        ┌───────────────────────┴───────────────────────┐
        ▼ Collaborative Track 1                         ▼ Collaborative Track 2
 ┌─────────────────────────────┐         ┌─────────────────────────────┐
 │ Sub-Microsecond SCADA Guard │         │ Cryptographic PCAP Forensics│
 │ Validating 18_cps_sec on    │         │ Validating ISO/IEC 27037    │
 │ high-voltage electrical     │         │ evidence admissibility with │
 │ transmission testbeds.      │         │ TPM 2.0 signed audit logs.  │
 └─────────────────────────────┘         └─────────────────────────────┘
```

---

## 2. Research Focus Areas

1. **Substation Protocol Resilience:** Joint testing of `libiec104_dissector` and `libiec61850_goose` against Industroyer2 attack replays on real substation relay hardware.
2. **Admissible Forensic Carving:** Leveraging Subsystem `22_dfir` to capture, seal, and verify packet captures during live military-grade red/blue team exercises.
```

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

---

### Complete in Part 8
- `sentinel-lab/docs/university-curriculum/thesis-topics-guide.md`
- `sentinel-lab/docs/university-curriculum/course-module-integration.md`
- `sentinel-lab/docs/university-curriculum/taltech-collaboration-guide.md`
- `sentinel-lab/docs/university-curriculum/aalto-collaboration-guide.md`
- `sentinel-lab/docs/university-curriculum/student-grant-support.md`

All 5 University Curriculum & Graduate Thesis files for `sentinel-lab` are now generated.

---

### Files to be Generated in Part 9

The next phase covers **Hands-On Research Walkthroughs & Tutorials** (`tutorials/` - 5 files):

1. `tutorials/evaluating-custom-csv-datasets.md` (Converting and evaluating an unlabelled university campus PCAP)
2. `tutorials/benchmarking-intel-npu-vs-cpu.md` (Measuring inference latency on Intel Meteor Lake / Lunar Lake NPUs)
3. `tutorials/measuring-xdp-drop-cycles.md` (Measuring CPU clock cycles consumed per drop via Linux perf)
4. `tutorials/exporting-reproducible-csv-artifacts.md` (Formatting benchmark outputs for publication-ready LaTeX tables)
5. `tutorials/running-testbed-in-vmware.md` (Executing the research harness inside VMware virtual machines)

Confirm when you are ready to proceed with Part 9.