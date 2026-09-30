# Protocol Validation Checks

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Validating the 0x534C4142 magic header and payload bounds.

## Magic

First word must equal 0x534C4142 or the frame is foreign.

## Bounds

Dim × 4 must fit the received length or the frame drops.

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
