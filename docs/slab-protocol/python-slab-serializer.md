# Python SLAB Dataset Serializer (`tools/csv_to_slab.py`)

`sentinel-lab` includes a Python utility to convert arbitrary research CSV datasets into the binary SLAB format for wire replay or offline file streaming.

---

## 1. Script Usage

```bash
python3 tools/csv_to_slab.py \
    --input-csv /tmp/cic_ids_2017_portscan.csv \
    --output-slab /tmp/benchmark_corpus.slab \
    --label-column "Label" \
    --positive-label "PortScan" \
    --dimensions 32
```

---

## 2. Serializer Implementation (`tools/csv_to_slab.py`)

```python
#!/usr/bin/env python3
import struct
import argparse
import pandas as pd
import numpy as np

SLAB_MAGIC = 0x534C4142 # ASCII: "SLAB"

def serialize_csv_to_slab(input_csv: str, output_slab: str, label_col: str, pos_label: str, target_dim: int):
    print(f"[*] Ingesting {input_csv}...")
    df = pd.read_csv(input_csv)

    # 1. Extract and Binarize Labels
    labels = (df[label_col].astype(str) == pos_label).astype(np.uint32).values
    feature_df = df.drop(columns=[label_col])

    # 2. Slice to Target Dimensions
    numeric_data = feature_df.select_dtypes(include=[np.number]).values[:, :target_dim]
    
    # 3. Min-Max Normalization into [-1.0, 1.0]
    mins = np.nanmin(numeric_data, axis=0)
    maxs = np.nanmax(numeric_data, axis=0)
    denom = np.where((maxs - mins) == 0, 1.0, (maxs - mins))
    normalized = 2.0 * ((numeric_data - mins) / denom) - 1.0
    normalized = np.nan_to_num(normalized, nan=0.0).astype(np.float32)

    num_samples = len(normalized)
    print(f"[*] Serializing {num_samples} flows into binary SLAB format...")

    with open(output_slab, "wb") as f_out:
        for idx in range(num_samples):
            # Header: magic (4B), event_id (8B), ground_truth (4B), dimensions (4B), flags (4B)
            header = struct.pack(
                "=IQII I",
                SLAB_MAGIC,
                idx + 1,
                labels[idx],
                target_dim,
                0 # Flags
            )
            # Tensor: D * Float32
            payload = normalized[idx].tobytes()
            f_out.write(header + payload)

    print(f"[+] Complete. Serialized file saved to: {output_slab}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert CSV datasets to SLAB binary format.")
    parser.add_argument("--input-csv", required=True)
    parser.add_argument("--output-slab", required=True)
    parser.add_argument("--label-column", default="Label")
    parser.add_argument("--positive-label", default="Attack")
    parser.add_argument("--dimensions", type=int, default=32)
    args = parser.parse_args()

    serialize_csv_to_slab(args.input_csv, args.output_slab, args.label_column, args.positive_label, args.dimensions)
```

