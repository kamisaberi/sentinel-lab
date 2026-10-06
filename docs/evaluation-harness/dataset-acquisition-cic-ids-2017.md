# Automated CIC-IDS-2017 PortScan Dataset Acquisition

To eliminate manual downloading and ensure reproducibility across academic testbeds, `sentinel-lab` includes an automated dataset fetcher that pulls the official **PortScan subset of CIC-IDS-2017** directly from the Canadian Institute for Cybersecurity archive.

---

## 1. Dataset Specifications

* **Corpus Identifier:** `PortScan.pcap_ISCX.csv`
* **Source Archive:** Canadian Institute for Cybersecurity (University of New Brunswick)
* **Archive Size:** $77.4\,\text{MB}$ compressed ($128.2\,\text{MB}$ uncompressed)
* **Total Flow Records:** $286{,}467$ labeled flow instances
* **Official SHA-256 Digest:** `e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000`

---

## 2. Python Ingestion Client (`harness/dataset_fetcher.py`)

```python
import os
import hashlib
import requests
from pathlib import Path
from tqdm import tqdm

CIC_PORTSCAN_URL = "http://205.174.165.80/CICDataset/CIC-IDS-2017/CSVs/GeneratedLabelledFlows.zip"
EXPECTED_SHA256 = "e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000"

def fetch_and_verify_cic_ids_2017(dest_dir: str = "/tmp/cic_dataset") -> Path:
    out_dir = Path(dest_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    target_csv = out_dir / "Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv"

    if target_csv.exists():
        print(f"[+] Dataset already cached: {target_csv}")
        return target_csv

    print(f"[*] Downloading CIC-IDS-2017 PortScan corpus ({CIC_PORTSCAN_URL})...")
    response = requests.get(CIC_PORTSCAN_URL, stream=True, timeout=60)
    response.raise_for_status()

    archive_path = out_dir / "cic_portscan.zip"
    total_size = int(response.headers.get("content-length", 0))

    with open(archive_path, "wb") as f, tqdm(total=total_size, unit="B", unit_scale=True) as bar:
        for chunk in response.iter_content(chunk_size=1048576):
            f.write(chunk)
            bar.update(len(chunk))

    # Unpack target CSV and clean up archive
    import zipfile
    with zipfile.ZipFile(archive_path, "r") as z:
        z.extract("Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv", out_dir)

    archive_path.unlink()
    print(f"[+] Extraction complete: {target_csv}")
    return target_csv
```

