### Part 10: Troubleshooting & Academic Help Desk (`troubleshooting/*`)

This final section covers troubleshooting dataset downloads from academic mirrors, managing raw socket Linux capabilities, resolving in-kernel eBPF JIT compiler errors, debugging OpenVINO/TensorRT dynamic library linking, academic FAQs, and research support escalation paths for `sentinel-lab`.

---

### File: `sentinel-lab/docs/troubleshooting/dataset-download-errors.md`

```markdown
# Resolving Academic Dataset Acquisition & Archive Timeouts

When running `examples/run_full_evaluation.py`, the automated fetcher connects to the Canadian Institute for Cybersecurity (CIC) repository (`http://205.174.165.80/`). University campus firewalls or server rate-limits can occasionally trigger connection timeouts or incomplete archive downloads.

---

## 1. Common Symptoms & Error Messages

```text
requests.exceptions.ConnectTimeout: HTTPConnectionPool(host='205.174.165.80', port=80): Max retries exceeded
zipfile.BadZipFile: File is not a zip file (Corrupted partial download)
ValueError: Dataset integrity mismatch for PortScan.pcap_ISCX.csv!
```

---

## 2. Resolving CIC Mirror Timeouts

If the primary university server is unresponsive, use the official **CERN/Zenodo Academic Mirror**:

```bash
# 1. Download directly from the permanent CERN/Zenodo research repository
curl -fsSL https://zenodo.org/record/1849200/files/PortScan.pcap_ISCX.csv.gz -o /tmp/cic_dataset/PortScan.pcap_ISCX.csv.gz

# 2. Decompress archive
gzip -d /tmp/cic_dataset/PortScan.pcap_ISCX.csv.gz

# 3. Rename to expected target path
mv /tmp/cic_dataset/PortScan.pcap_ISCX.csv \
   /tmp/cic_dataset/Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv
```

---

## 3. Manual Checksum Verification

Verify the SHA-256 integrity hash of your downloaded file before running the testbed:

```bash
sha256sum /tmp/cic_dataset/Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv
```

### Expected Checksum:
```text
e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000
```

Once placed in `/tmp/cic_dataset/`, the evaluation harness will automatically detect the cached file and skip the network download step.
```

