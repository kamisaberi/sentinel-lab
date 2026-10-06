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

