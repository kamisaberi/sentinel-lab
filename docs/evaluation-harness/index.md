# Evaluation Harness & Scientific Modules

The evaluation harness orchestrates dataset acquisition, flow normalization, raw socket injection, and scientific metric calculation.

## Section Navigation

- [Harness Architecture](./harness-architecture.md): End-to-end orchestration sequence in `examples/run_full_evaluation.py`.
- [14 Scientific Benchmark Modules](./14-scientific-benchmark-modules.md): Detailed scientific guide covering all 14 research modules (`hil_coupler`, `wcet_engine`, `pqc_overhead`, etc.).
- [EVT WCET Latency Bound](./evt-wcet-latency-bound.md): Extreme Value Theory Gumbel fitting proving the $p99.999\%$ Worst-Case Execution Time ($1.82\,\mu\text{s}$).
- [CIC-IDS-2017 Dataset](./dataset-acquisition-cic-ids-2017.md): Automated dataset fetcher (`dataset_fetcher.py`) for the PortScan corpus.
- [Flow Normalization](./flow-normalization-pipeline.md): Logarithmic pre-scaling, MinMax transformation into $[-1.0, 1.0]$, and float clamping.
- [Raw Socket Injection](./wire-speed-raw-socket-injection.md): High-speed packet injection over `AF_PACKET` raw sockets ($> 60\text{k EPS}$).
- [Metrics Calculation](./metrics-calculation.md): Confusion matrix derivation, Precision, Recall, and F1-score calculation.
- [Percentile Latency Profiler](./percentile-latency-profiler.md): High-resolution latency profiler and LaTeX table exporter.
