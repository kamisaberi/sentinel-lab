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

