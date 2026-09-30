# Ten-Minute Quickstart

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


From git clone to empirical F1 and latency metrics in ten minutes.

## Clock

Most time is dataset download; the build itself takes minutes.

## Result

A CSV with accuracy, F1, and p50–p99.9 ready for your paper.

```bash
git clone https://github.com/kamisaberi/sentinel-lab.git
cd sentinel-lab && ./bpf/build_bpf.sh
sudo ./sentinel_lab &   # terminal 1
python3 examples/run_full_evaluation.py   # terminal 2
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
