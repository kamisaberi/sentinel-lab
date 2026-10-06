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

