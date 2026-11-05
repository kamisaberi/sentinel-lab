# SLAB Universal Binary Wire Protocol

The SLAB (Sentinel-Lab) protocol is an open, self-describing binary wire format (`0x534C4142`) embedding feature tensors and ground-truth labels inside Ethernet frames.

## Section Navigation

- [Protocol Specification](./protocol-specification.md): Header framing, 24-byte layout, and field definitions.
- [Bytefield Wire Layout](./bytefield-wire-layout.md): Packed C++20 data structures (`slab_protocol.hpp`) and hexadecimal wire dumps.
- [Zero-Copy Casting](./zero-copy-casting.md): In-place memory mapping via `std::span` bypassing user-space allocations.
- [Multi-Dataset Compatibility](./multi-dataset-compatibility.md): Supporting CIC-IDS-2017, UNSW-NB15, and SCADA Triton datasets.
- [Python SLAB Serializer](./python-slab-serializer.md): `tools/csv_to_slab.py` utility for serializing custom research CSVs.
- [Protocol Validation Checks](./protocol-validation-checks.md): Truncation checks, dimension sanity bounds, and eBPF kernel verifier safety.
