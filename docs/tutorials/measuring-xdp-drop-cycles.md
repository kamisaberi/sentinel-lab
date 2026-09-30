# Measuring XDP Drop Cycles

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Measuring CPU clock cycles consumed per drop via Linux perf.

## Method

perf stat on the drop path; RDTSC cross-checks.

## Budget

Sub-120-cycle verdicts leave room for parsing growth.

```bash
$ sudo perf stat -e cycles,instructions ./build/smoke_drop
# expect < 120 cycles per verdict
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
