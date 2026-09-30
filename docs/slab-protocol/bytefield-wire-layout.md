# Bytefield Wire Layout

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Formal 24-byte header layout and 32-bit word alignment.

## Alignment

Every field lands on 4-byte boundaries; no packing pragmas needed.

## Diagram

Magic | EventID | Label | Dim | Tensor — offsets 0x00–0x14.

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
