---

### File: `sentinel-lab/docs/tutorials/exporting-reproducible-csv-artifacts.md`

```markdown
# Formatting Benchmark Outputs for Publication-Ready LaTeX Tables

Scientific reviewers require verifiable empirical outputs. `sentinel-lab` includes an automated metric exporter that transforms raw nanosecond benchmark logs into structured CSVs and publication-formatted LaTeX tables.

---

## 1. Generating Raw Timing CSVs

Run the benchmark harness with the `--export-raw-csv` option:

```bash
sudo ./build/bin/sentinel_lab \
    --slab-file /tmp/cic_ids_2017.slab \
    --export-raw-csv /tmp/raw_latencies.csv \
    --samples 50000
```

### Resulting Raw CSV Structure (`raw_latencies.csv`)
```text
event_id,ground_truth,predicted_class,latency_cycles,latency_us,drop_enforced
1,1,1,1440,0.72,1
2,0,0,1400,0.70,0
3,1,1,1480,0.74,1
```

---

## 2. Converting CSV to LaTeX (`tools/csv_to_latex.py`)

Run the LaTeX formatter:

```bash
python3 tools/csv_to_latex.py \
    --input-csv /tmp/raw_latencies.csv \
    --output-tex paper/tables/results.tex \
    --model-name "Sentinel-Lab (XDP + NPU)"
```

### Generated LaTeX Output (`paper/tables/results.tex`)

```latex
\begin{table}[h]
\centering
\caption{Empirical Classification Accuracy and Latency Distribution}
\label{tab:empirical_results}
\begin{tabular}{lcccccc}
\toprule
\textbf{Architecture} & \textbf{Accuracy} & \textbf{F1} & \textbf{p50 ($\mu$s)} & \textbf{p90 ($\mu$s)} & \textbf{p99 ($\mu$s)} & \textbf{p99.9 ($\mu$s)} \\
\midrule
Sentinel-Lab (XDP + NPU) & 99.88\% & 0.9988 & 0.72 & 0.78 & 0.84 & 0.91 \\
\bottomrule
\end{tabular}
\end{table}
```

Include this file directly in `paper/paper.tex` via `\input{tables/results.tex}` for automated document builds.
```

---

### File: `sentinel-lab/docs/tutorials/running-testbed-in-vmware.md`

```markdown
# Executing the Research Harness Inside VMware Virtual Machines

Graduate students without access to physical multi-NIC bare-metal servers can execute `sentinel-lab` inside **VMware Workstation Pro, VMware Fusion, or VMware vSphere ESXi**.

---

## 1. Virtual Machine Hardware Prerequisites

Configure the virtual machine settings:
* **CPU:** 4 vCPUs with **"Virtualize Intel VT-x/EPT or AMD-V/RVI"** enabled.
* **RAM:** 8 GB RAM (100% Reserved, zero memory ballooning).
* **Virtual Adapter:** Set network adapter type to **`vmxnet3`**.
* **Virtual Disk:** NVMe Virtual Disk Controller.

---

## 2. Tuning VMware Virtual Interfaces for eBPF

By default, the Linux `vmxnet3` driver enables Large Receive Offload (LRO), which blocks native XDP hooks. Run these preparation commands inside the virtual machine before running the testbed:

```bash
# 1. Disable offloads that conflict with XDP
sudo ethtool -K ens33 lro off gro off rxvlan off txvlan off

# 2. Expand virtual receive queues
sudo ethtool -G ens33 rx 4096 tx 4096

# 3. Set standard MTU
sudo ip link set dev ens33 mtu 1500
```

---

## 3. Running the Testbed in Generic Mode

If your hypervisor does not support Native Driver mode on the virtual network adapter, pass `--xdp-mode SKB` to run in Generic XDP mode:

```bash
sudo python3 examples/run_full_evaluation.py \
    --interface ens33 \
    --xdp-mode SKB \
    --samples 10000
```

### Expected Output
```text
[*] Attached XDP filter to ens33 in Generic SKB mode.
[+] Baseline evaluation active: Latency p50: 2.45 µs | F1-Score: 0.9988
[+] Research harness verified inside virtualized VMware guest!
```
```

