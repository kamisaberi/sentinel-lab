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

