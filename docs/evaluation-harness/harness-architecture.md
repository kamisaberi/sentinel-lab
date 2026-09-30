### Part 6: Autonomous Evaluation Harness (`evaluation-harness/*`)

This section contains 6 technical implementation guides detailing the automated benchmarking pipeline in `sentinel-lab`: the architecture of `run_full_evaluation.py`, automated CIC-IDS-2017 dataset acquisition, flow normalization math, raw socket packet injection, classification metric derivation, and empirical latency distribution profiling.

---

### File: `sentinel-lab/docs/evaluation-harness/harness-architecture.md`

```markdown
# Autonomous Benchmark Harness Architecture (`run_full_evaluation.py`)

`examples/run_full_evaluation.py` is the primary entry point for executing reproducible scientific benchmarks in `sentinel-lab`. It orchestrates dataset acquisition, flow feature normalization, binary SLAB wire serialization, raw socket packet injection, and empirical metric generation within a single automated pipeline.

---

## 1. End-to-End Orchestration Sequence

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. DATASET ACQUISITION (dataset_fetcher.py)                 │
 │   - Downloads official PortScan.pcap_ISCX.csv (77.4 MB)     │
 │   - Validates SHA-256 integrity digest                      │
 └──────────────────────────────┬──────────────────────────────┘
                                │
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 2. NORMALIZATION & CLEANING (normalizer.py)                 │
 │   - Slices 32 continuous network features                   │
 │   - Applies robust MinMax scaling into [-1.0, 1.0]          │
 └──────────────────────────────┬──────────────────────────────┘
                                │
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 3. BINARY SLAB SERIALIZATION (csv_to_slab.py)               │
 │   - Packs frames into 0x534C4142 format with Ground Truth   │
 └──────────────────────────────┬──────────────────────────────┘
                                │
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 4. HARDWARE SOCKET BLASTER (socket_injector.py)             │
 │   - Blasts binary packets over AF_PACKET at > 60k EPS       │
 └──────────────────────────────┬──────────────────────────────┘
                                │
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 5. HARNESS VALIDATION ENGINE (sentinel_lab)                 │
 │   - eBPF in-kernel driver drops vs. AI classification       │
 │   - Collects exact hardware execution cycle counts          │
 └──────────────────────────────┬──────────────────────────────┘
                                │
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 6. REPORT GENERATION (metrics_exporter.py)                  │
 │   - Derives Confusion Matrix, F1-Score, and Latency CDF     │
 │   - Generates publication-ready paper/tables/results.tex    │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. CLI Invocation Syntax

```bash
sudo python3 examples/run_full_evaluation.py \
    --interface lo \
    --samples 10000 \
    --target-silicon AUTO \
    --batch-size 1 \
    --export-latex paper/tables/results.tex
```

### Command Flags

| Flag | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `--interface` | String | `lo` | Target network adapter for raw packet injection. |
| `--samples` | Integer | `50000` | Total flow records to evaluate from the dataset. |
| `--batch-size` | Integer | `1` | Evaluation batch size ($N=1$ for line-rate fast path). |
| `--target-silicon` | String | `AUTO` | Silicon backend (`AUTO`, `OPENVINO`, `TENSORRT`). |
| `--export-latex` | File Path | `paper/tables/results.tex` | Destination for compiled LaTeX table artifact. |
```

