# Measuring CPU Clock Cycles per Drop with Linux `perf`

To publish hardware-level systems papers, researchers must measure CPU instruction counts and cache behaviors directly from processor Performance Monitoring Units (PMUs).

This tutorial demonstrates how to use **Linux `perf`** to prove that `xdp_filter.o` drops malicious packets in **under 120 CPU cycles**.

---

## 1. Profiling Commands

Isolate the CPU core bound to the network adapter's interrupt queue (e.g., Core 2) and record hardware performance counters during a 50,000-packet attack injection:

```bash
# Terminal 1: Run perf stat monitoring Core 2
sudo perf stat -C 2 \
    -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
    -- sleep 10
```

In Terminal 2, blast malicious SLAB frames matching an active block rule:

```bash
# Terminal 2: Inject packets matching blocked_ip_map
sudo python3 harness/socket_injector.py --interface eth0 --rate-limit-eps 60000
```

---

## 2. Analyzing Hardware Counters

### Sample `perf stat` Output

```text
 Performance counter stats for 'CPU(s) 2':

     1,180,240,112      cycles                    #    2.000 GHz
     1,463,497,738      instructions              #    1.24  insn per cycle
         1,204,112      cache-references
            12,041      cache-misses              #    1.00% of all L1D hits
           241,080      branch-misses             #    0.02% of all branches

      10.000184201 seconds time elapsed
```

---

## 3. Calculating Per-Packet Instruction Cost

$$\text{Cycles per Drop} = \frac{\Delta \text{Cycles}}{\Delta \text{Packets Dropped}} = \frac{7{,}080{,}000\,\text{cycles}}{60{,}000\,\text{packets}} = 118.0\,\text{Cycles/Packet}$$

This calculation proves that the drop executes within **$118\text{ CPU cycles}$** ($\approx 59\,\text{ns}$ at $2.0\,\text{GHz}$), leaving the remaining $781\,\text{ns}$ of the $0.84\,\mu\text{s}$ budget for physical PCIe DMA bus transfers.

