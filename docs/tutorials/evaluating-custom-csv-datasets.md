# Converting & Evaluating a Custom University Campus PCAP

This tutorial demonstrates how graduate students and researchers can capture raw network traffic from a university campus subnet, extract continuous 32-dimensional flow vectors, serialize them into the SLAB binary wire protocol, and evaluate classification performance.

---

## 1. Experimental Pipeline

```text
 [ Campus Network Tap / Switch SPAN ]
                 │
                 ▼ tcpdump -i eth0 -w campus_traffic.pcap
 [ Raw Network PCAP (e.g. 500 MB) ]
                 │
                 ▼ python3 tools/pcap_extractor.py
 [ campus_flows.csv (32 Continuous Feature Columns) ]
                 │
                 ▼ python3 tools/csv_to_slab.py
 [ campus_traffic.slab (0x534C4142 Wire Frames) ]
                 │
                 ▼ Raw Socket Injection / sentinel_lab
 [ Hardware Evaluation: Confusion Matrix & Latency Distributions ]
```

---

## 2. Step 1: Capture and Extract Flow Features

Capture a traffic slice from your test network:

```bash
sudo tcpdump -i eth0 -c 50000 -w /tmp/campus_traffic.pcap
```

Extract the 32 continuous flow features matching the SLAB specification:

```bash
python3 -m tools.pcap_extractor \
    --input-pcap /tmp/campus_traffic.pcap \
    --output-csv /tmp/campus_flows.csv \
    --bidirectional
```

---

## 3. Step 2: Serialize to Binary SLAB Protocol

Convert the CSV into the self-describing binary format using `tools/csv_to_slab.py`:

```bash
python3 tools/csv_to_slab.py \
    --input-csv /tmp/campus_flows.csv \
    --output-slab /tmp/campus_traffic.slab \
    --label-column "Label" \
    --positive-label "Malicious" \
    --dimensions 32
```

Verify that the binary header matches the `0x534C4142` magic token:

```bash
hexdump -C /tmp/campus_traffic.slab | head -n 2
# Output: 00000000  53 4c 41 42 ... |SLAB...|
```

---

## 4. Step 3: Execute Hardware Evaluation

Stream the generated binary dataset through the testbed engine:

```bash
sudo ./build/bin/sentinel_lab \
    --slab-file /tmp/campus_traffic.slab \
    --model-path models/network_threat_v2.onnx \
    --target-silicon AUTO \
    --export-latex paper/tables/campus_results.tex
```

