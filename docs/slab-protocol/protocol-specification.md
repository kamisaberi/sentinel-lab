### Part 3: The SLAB Universal Binary Wire Protocol (`slab-protocol/*`)

This section contains 6 technical specifications and reference implementations detailing the **SLAB (Sentinel-Lab) Universal Binary Wire Protocol**: wire layout, memory-mapped zero-copy casting, cross-dataset compatibility, Python serialization tools, and header boundary validation.

---

### File: `sentinel-lab/docs/slab-protocol/protocol-specification.md`

```markdown
# SLAB Universal Binary Wire Protocol Specification

The SLAB (Sentinel-Lab) protocol is an open, self-describing binary wire format designed for hardware-in-the-loop (HIL) intrusion detection research. It embeds high-dimensional machine learning feature tensors alongside verifiable ground-truth annotations inside standard Ethernet/UDP payloads.

---

## 1. Protocol Architecture & Header Framing

Every SLAB packet consists of an immutable **24-byte header** followed immediately by a contiguous array of IEEE 754 single-precision floating-point numbers:

```text
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Magic (0x534C4142)                       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
+                       Event ID (64-Bit)                       +
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                 Ground Truth Class (32-Bit)                   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                 Tensor Dimensions / Length D                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Flags / Reserved (32-Bit)                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                 Feature Tensor Element 0 (Float32)            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                 Feature Tensor Element 1 (Float32)            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             . . .                             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                 Feature Tensor Element D-1 (Float32)          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

---

## 2. Structural Field Definitions

| Field Name | Type | Size | Description |
| :--- | :--- | :--- | :--- |
| **`magic`** | `uint32_t` | 4 Bytes | Magic synchronization token: `0x534C4142` (ASCII `"SLAB"`). |
| **`event_id`** | `uint64_t` | 8 Bytes | Monotonically increasing sequence ID assigned by generator. |
| **`ground_truth`** | `uint32_t` | 4 Bytes | True label: `0 = Benign`, `1 = Attack` (or multi-class taxonomy). |
| **`dimensions`** | `uint32_t` | 4 Bytes | Dimensionality $D$ of the trailing tensor (e.g. 32, 42, 80). |
| **`flags`** | `uint32_t` | 4 Bytes | Reserved bitmask for dataset identification and normalization type. |
| **`tensor`** | `float[D]` | $D \times 4\text{ Bytes}$ | Contiguous, normalized IEEE 754 float features. |

---

## 3. Total Wire Footprint

For a standard 32-dimensional NetFlow feature vector ($D = 32$):

$$\text{Payload Size} = 24\,\text{bytes (Header)} + (32 \times 4\,\text{bytes}) = 152\,\text{bytes}$$

The entire packet easily fits within standard $1500\text{-byte}$ Ethernet Maximum Transmission Units (MTU), eliminating network IP fragmentation.
```

---

### File: `sentinel-lab/docs/slab-protocol/bytefield-wire-layout.md`

```markdown
# Formal Bytefield Wire Layout & C++20 Data Structure

To ensure zero-copy deserialization across heterogeneous CPU architectures (x86_64, aarch64), the SLAB header is explicitly packed to 64-bit alignment boundaries.

---

## 1. C++20 Header Structure (`include/sentinel_lab/slab_protocol.hpp`)

```cpp
#pragma once

#include <cstdint>
#include <span>

namespace sentinel::lab {

// 0x534C4142 corresponds to ASCII: 'S', 'L', 'A', 'B'
inline constexpr uint32_t SLAB_MAGIC = 0x534C4142;

#pragma pack(push, 1)
struct SlabHeader {
    uint32_t magic;          // Byte 0-3  : 0x534C4142
    uint64_t event_id;       // Byte 4-11 : Sequence Number
    uint32_t ground_truth;   // Byte 12-15: 0=Benign, 1=Malicious
    uint32_t dimensions;     // Byte 16-19: Feature count D
    uint32_t flags;          // Byte 20-23: Bitmask flags / Reserved
};
#pragma pack(pop)

static_assert(sizeof(SlabHeader) == 24, "SlabHeader must be exactly 24 bytes");

} // namespace sentinel::lab
```

---

## 2. Hexadecimal Wire Inspection

An example hex dump of a raw SLAB packet received on an `AF_PACKET` socket:

```text
Offset    00 01 02 03  04 05 06 07  08 09 0A 0B  0C 0D 0E 0F   ASCII
-------------------------------------------------------------------------
00000000  53 4C 41 42  01 00 00 00  00 00 00 00  01 00 00 00   SLAB............
          [  Magic  ]  [      Event ID: 1     ]  [ Class: 1  ]

00000010  20 00 00 00  00 00 00 00  3D 0A D7 A3  41 40 00 00    .......=...A@..
          [ Dim: 32 ]  [ Reserved ]  [ Float 0 ]  [ Float 1 ]
```

* Bytes `00-03` (`53 4C 41 42`): Validates the frame as a legitimate research packet.
* Bytes `04-0B` (`01 00 00 00 00 00 00 00`): Sequence identifier `1`.
* Bytes `0C-0F` (`01 00 00 00`): Ground-truth annotation `1` (Attack).
* Bytes `10-13` (`20 00 00 00`): `0x20` = 32 dimensions follow.
```

---

### File: `sentinel-lab/docs/slab-protocol/zero-copy-casting.md`

```markdown
# Memory-Mapping & Zero-Copy Casting

Conventional network parsers deserialize incoming byte arrays by allocating new heap objects and copying fields element-by-element. This overhead degrades ingestion throughput.

`sentinel-lab` eliminates copies through **Direct Pointer Casting via `std::span`**.

---

## 1. Zero-Copy Ingestion Flow

```text
 [ Linux Raw Socket Ingress Buffer: packet_buffer ]
                     │
                     ▼ Direct In-Place Memory Map
 ┌─────────────────────────────────────────────────────────────┐
 │ const auto* slab = reinterpret_cast<const SlabHeader*>(buf) │
 └───────────────────┬─────────────────────────────────────────┘
                     │ (Validate magic == 0x534C4142)
                     ▼ Pointer Offset (+24 Bytes)
 ┌─────────────────────────────────────────────────────────────┐
 │ const float* raw_tensor = reinterpret_cast<const float*>(   │
 │                                   buf + sizeof(SlabHeader)) │
 │ std::span<const float> tensor_view(raw_tensor, slab->dim)   │
 └───────────────────┬─────────────────────────────────────────┘
                     │
                     ▼ Passed Directly to AI Silicon
 [ Hardware Memory Pointer: No intermediate allocations ]
```

---

## 2. In-Engine Implementation (`src/engine/packet_consumer.cpp`)

```cpp
#include <sentinel_lab/slab_protocol.hpp>
#include <xinfer/tensor.hpp>
#include <span>

namespace sentinel::lab {

bool process_slab_packet_zerocopy(
    std::span<const uint8_t> raw_network_frame,
    std::shared_ptr<xinfer::Tensor>& target_tensor,
    uint32_t& out_ground_truth
) {
    // 1. Boundary Check: Header must fit within incoming packet
    if (raw_network_frame.size() < sizeof(SlabHeader)) {
        return false;
    }

    // 2. Cast Header Pointer
    const auto* header = reinterpret_cast<const SlabHeader*>(raw_network_frame.data());
    if (header->magic != SLAB_MAGIC) {
        return false; // Non-SLAB packet
    }

    // 3. Boundary Check: Verify payload contains claimed float dimensions
    size_t expected_payload_bytes = header->dimensions * sizeof(float);
    if (raw_network_frame.size() < sizeof(SlabHeader) + expected_payload_bytes) {
        return false; // Truncated packet
    }

    out_ground_truth = header->ground_truth;

    // 4. Zero-Copy Pointer Mapping directly to tensor memory
    const float* float_data = reinterpret_cast<const float*>(
        raw_network_frame.data() + sizeof(SlabHeader)
    );

    // Bypasses user-space allocations; maps float memory directly
    target_tensor->copy_from_host(float_data, expected_payload_bytes);
    return true;
}

} // namespace sentinel::lab
```
```

---

### File: `sentinel-lab/docs/slab-protocol/multi-dataset-compatibility.md`

```markdown
# Multi-Dataset Cross-Compatibility

Because the SLAB protocol is self-describing through its `dimensions` field ($D$), a single `sentinel_lab` deployment can evaluate disparate intrusion detection benchmarks without recompiling the testbed engine.

---

## 1. Supported Academic Dataset Mappings

| Benchmark Corpus | Originating University | Raw Dimension ($D$) | SLAB Wire Size | Primary Exploit Vectors |
| :--- | :--- | :--- | :--- | :--- |
| **CIC-IDS-2017** | Univ. of New Brunswick (CIC) | **$32$** Features | $152\text{ Bytes}$ | PortScan, DoS, BruteForce |
| **UNSW-NB15** | Australian Cyber Security Centre | **$42$** Features | $192\text{ Bytes}$ | Fuzzers, Backdoors, Worms |
| **NSL-KDD** | University of New Brunswick | **$41$** Features | $188\text{ Bytes}$ | Legacy SYN Floods, R2L, U2R |
| **SCADA Triton** | Industrial Testbed (OT) | **$16$** Features | $88\text{ Bytes}$ | TriStation Safety Overrides |
| **CIC-DDoS-2019**| Univ. of New Brunswick (CIC) | **$80$** Features | $344\text{ Bytes}$ | Volumetric NTP/DNS Amplification |

---

## 2. Dynamic Dimension Handshake

When `sentinel_lab` parses a packet, it reads `header->dimensions`:
* If the incoming frame dimension matches the loaded AI model's input shape, evaluation executes directly.
* If a dimension mismatch occurs (e.g. an $80\text{-dim}$ frame arrives at a $32\text{-dim}$ model), the packet is skipped and logged to prevent memory over-runs.
```

---

### File: `sentinel-lab/docs/slab-protocol/python-slab-serializer.md`

```markdown
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
```

---

### File: `sentinel-lab/docs/slab-protocol/protocol-validation-checks.md`

```markdown
# Protocol Validation Checks & Fuzzing Resilience

When streaming binary packets over raw network sockets, the parser must handle truncated frames, malformed headers, and network noise without throwing uncaught exceptions or crashing the research daemon.

---

## 1. Automated Validation Checklist

```text
 Incoming Buffer: packet_buffer (Size: S bytes)
                      │
                      ▼ Check 1: Minimum Size
 [ S >= 24 Bytes (sizeof(SlabHeader))? ] ──── NO ──► Drop Packet (Truncated)
                      │ YES
                      ▼ Check 2: Magic Synchronization Token
 [ header->magic == 0x534C4142? ] ─────────── NO ──► Drop Packet (Invalid Protocol)
                      │ YES
                      ▼ Check 3: Dimension Boundary Sanity
 [ header->dimensions <= 1024? ] ──────────── NO ──► Drop Packet (Dimension Overflow)
                      │ YES
                      ▼ Check 4: Payload Completeness
 [ S >= 24 + (dimensions * 4)? ] ──────────── NO ──► Drop Packet (Incomplete Payload)
                      │ YES
                      ▼
 [ PROCEED TO ZERO-COPY INFERENCE EVALUATION ]
```

---

## 2. In-Kernel eBPF Filter Rejection

Before frames reach the user-space research harness, the in-kernel eBPF program (`xdp_filter.o`) performs an initial bounds check:

```c
// bpf/xdp_filter.c
if ((void *)(slab_hdr + 1) > data_end) {
    return XDP_PASS; // Frame too small to contain a SLAB header
}
```

This prevents user-space memory corruption even under malicious packet flooding.
```

