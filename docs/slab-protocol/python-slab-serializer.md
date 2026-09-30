# Python SLAB Serializer

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Using tools/csv_to_slab.py to convert custom research datasets.

## Usage

Point at a CSV, declare the label column, get a .slab stream.

## Checks

Serializer validates widths and emits a manifest.

```bash
$ python3 tools/csv_to_slab.py --in campus.csv --label col=12 \
    --out campus.slab
[+] wrote 1,204,118 frames, dim=32
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
