#!/usr/bin/env python3
"""
Full Autonomous Evaluation Pipeline:
  1. Auto-downloads real pre-trained ONNX model from Hugging Face.
  2. Auto-downloads real CIC-IDS-2017 network intrusion dataset.
  3. Preprocesses real flow features and ground-truth labels.
  4. Transmits live packets over the NIC socket using the SLAB protocol.
  5. Queries sentinel-lab API and displays benchmark results.
"""

import os
import sys
import time
import socket
import struct
import urllib.request
import requests
import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler

# =========================================================================
# CONFIGURATION & REPOSITORY URLS
# =========================================================================
TARGET_IP = "127.0.0.1"
TARGET_PORT = 9000
API_URL = "http://localhost:8443"

MODELS_DIR = "models"
MODEL_PATH = os.path.join(MODELS_DIR, "network_threat.onnx")
PRETRAINED_MODEL_URL = (
    "https://huggingface.co/darkknight25/ddos_xgboost_onnx/resolve/main/ddos_model.onnx?download=true"
)

DATASET_DIR = "tools"
DATASET_CSV = os.path.join(DATASET_DIR, "cicids2017_portscan.csv")
DATASET_URL = (
    "https://huggingface.co/datasets/c01dsnap/CIC-IDS2017/resolve/main/Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv"
)

# 32 Standard Network Flow Feature Columns from CIC-IDS-2017
FEATURE_COLS = [
    "Destination Port", "Flow Duration", "Total Fwd Packets", "Total Backward Packets",
    "Total Length of Fwd Packets", "Total Length of Bwd Packets", "Fwd Packet Length Max",
    "Fwd Packet Length Min", "Fwd Packet Length Mean", "Fwd Packet Length Std",
    "Bwd Packet Length Max", "Bwd Packet Length Min", "Bwd Packet Length Mean",
    "Bwd Packet Length Std", "Flow Bytes/s", "Flow Packets/s", "Flow IAT Mean",
    "Flow IAT Std", "Flow IAT Max", "Flow IAT Min", "Fwd IAT Total",
    "Fwd IAT Mean", "Fwd IAT Std", "Fwd IAT Max", "Fwd IAT Min",
    "Bwd IAT Total", "Bwd IAT Mean", "Bwd IAT Std", "Bwd IAT Max",
    "Bwd IAT Min", "Fwd PSH Flags", "Bwd PSH Flags"
]

# ANSI Terminal Colors (Zero Emojis)
GREEN  = "\033[0;32m"
YELLOW = "\033[1;33m"
RED    = "\033[0;31m"
BLUE   = "\033[0;34m"
NC     = "\033[0m"


# =========================================================================
# STEP 1: DOWNLOAD PRE-TRAINED ONNX MODEL
# =========================================================================
def download_pretrained_model():
    os.makedirs(MODELS_DIR, exist_ok=True)
    if os.path.exists(MODEL_PATH) and os.path.getsize(MODEL_PATH) > 0:
        print(f"{GREEN}[Step 1] Using cached pre-trained ONNX model: {MODEL_PATH} ({os.path.getsize(MODEL_PATH)} bytes){NC}")
        return

    print(f"{YELLOW}[Step 1] Downloading pre-trained ONNX threat model from Hugging Face...{NC}")
    try:
        urllib.request.urlretrieve(PRETRAINED_MODEL_URL, MODEL_PATH)
        print(f"{GREEN}Model download complete: {MODEL_PATH} ({os.path.getsize(MODEL_PATH)} bytes){NC}")
    except Exception as e:
        print(f"{RED}Error downloading ONNX model: {e}{NC}")
        sys.exit(1)


# =========================================================================
# STEP 2: DOWNLOAD REAL CIC-IDS-2017 DATASET
# =========================================================================
def download_dataset():
    os.makedirs(DATASET_DIR, exist_ok=True)
    if os.path.exists(DATASET_CSV) and os.path.getsize(DATASET_CSV) > 1000000:
        print(f"{GREEN}[Step 2] Using cached CIC-IDS-2017 dataset: {DATASET_CSV}{NC}")
        return

    print(f"{YELLOW}[Step 2] Downloading real CIC-IDS-2017 PortScan dataset (77 MB)...{NC}")
    try:
        resp = requests.get(DATASET_URL, stream=True)
        resp.raise_for_status()
        total = int(resp.headers.get('content-length', 0))
        downloaded = 0

        with open(DATASET_CSV, 'wb') as f:
            for chunk in resp.iter_content(chunk_size=1024 * 1024):
                if chunk:
                    f.write(chunk)
                    downloaded += len(chunk)
                    if total > 0:
                        pct = (downloaded / total) * 100
                        print(f"\r  Downloaded: {downloaded / (1024*1024):.1f} MB / {total / (1024*1024):.1f} MB ({pct:.1f}%)", end='', flush=True)
        print(f"\n{GREEN}Dataset download complete!{NC}")
    except Exception as e:
        print(f"\n{RED}Error downloading dataset: {e}{NC}")
        sys.exit(1)


# =========================================================================
# STEP 3: PREPROCESS REAL TRAFFIC AND LABELS
# =========================================================================
def load_and_preprocess_dataset(max_samples=10000):
    print(f"\n{YELLOW}[Step 3] Parsing and normalizing real network flows...{NC}")
    df = pd.read_csv(DATASET_CSV, low_memory=False)
    df.columns = df.columns.str.strip()
    df.replace([np.inf, -np.inf], np.nan, inplace=True)
    df.dropna(subset=FEATURE_COLS + ['Label'], inplace=True)

    # Ground truth: 0 = Benign traffic, 1 = Real attack (PortScan)
    labels = (df['Label'] != 'BENIGN').astype(np.int32).values
    raw_features = df[FEATURE_COLS].astype(np.float32).values

    # Normalize features using real dataset statistics
    scaler = StandardScaler()
    features = scaler.fit_transform(raw_features).astype(np.float32)

    total_count = min(len(features), max_samples)
    features = features[:total_count]
    labels = labels[:total_count]

    benign_count = int(np.sum(labels == 0))
    attack_count = int(np.sum(labels == 1))

    print(f"{GREEN}Preprocessed {total_count} real network flows:{NC}")
    print(f"  - Real Benign Packets  : {benign_count}")
    print(f"  - Real Attack Packets  : {attack_count}")
    return features, labels


# =========================================================================
# STEP 4: STREAM PACKETS OVER NIC WIRE PROTOCOL TO SENTINEL-LAB
# =========================================================================
def stream_packets_to_sentinel(features, labels):
    total_samples = len(features)
    num_features = features.shape[1]

    print(f"\n{YELLOW}[Step 4] Streaming {total_samples} packets to Sentinel-Lab ({TARGET_IP}:{TARGET_PORT})...{NC}")
    print("Wire Protocol: [Magic (SLAB) | EventID | GroundTruth | NumFeatures | Feature Payload]")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    t0 = time.time()
    sent = 0

    for i in range(total_samples):
        event_id = i + 1
        ground_truth = int(labels[i])
        feature_bytes = features[i].tobytes()

        # Dynamic Universal Header:
        # Magic (4s) | EventID (uint64) | GroundTruth (int32) | NumFeatures (uint32) | Floats
        packet_payload = struct.pack("!4sQii", b"SLAB", event_id, ground_truth, num_features) + feature_bytes
        sock.sendto(packet_payload, (TARGET_IP, TARGET_PORT))
        sent += 1

        # Throttle slightly to simulate high-rate stream without local socket overflow
        if sent % 100 == 0:
            time.sleep(0.001)

    duration = time.time() - t0
    rate = sent / duration if duration > 0 else 0
    print(f"{GREEN}Transmission finished: {sent} real packets delivered in {duration:.2f}s ({rate:.2f} EPS).{NC}")
    sock.close()


# =========================================================================
# STEP 5: QUERY LIVE TELEMETRY AND BENCHMARK RESULTS
# =========================================================================
def query_results():
    print(f"\n{BLUE}[Step 5] Querying Sentinel-Lab live telemetry...{NC}")
    time.sleep(1) # Allow C++ worker queue to finalize
    try:
        resp = requests.get(f"{API_URL}", timeout=2)
        if resp.status_code == 200:
            data = resp.json()
            print(f"{GREEN}=================================================================={NC}")
            print(f"{GREEN}          SENTINEL-LAB LIVE HARDWARE EVALUATION RESULTS           {NC}")
            print(f"{GREEN}=================================================================={NC}")
            print(f"Total Packets Processed : {data.get('total_events')}")
            print(f"Sustained Throughput    : {data.get('throughput_eps', 0):.2f} EPS")
            print(f"Mean Latency            : {data.get('mean_latency_us', 0):.2f} microseconds (us)")
            print(f"P95 Latency             : {data.get('p95_latency_us', 0):.2f} microseconds (us)")
            print(f"P99 Latency             : {data.get('p99_latency_us', 0):.2f} microseconds (us)")
            print(f"Active Blocked IPs      : {data.get('blocked_ips_count')}")
            print(f"{GREEN}=================================================================={NC}")
    except Exception as e:
        print(f"{YELLOW}Notice: Could not poll HTTP API ({e}). Check C++ terminal output for full statistics.{NC}")


# =========================================================================
# MAIN EXECUTION ENTRY POINT
# =========================================================================
def main():
    print("==================================================================")
    print("  Sentinel-Lab: Real Dataset & Pretrained Model Evaluation Demo   ")
    print("==================================================================")

    # 1. Download pre-trained model
    download_pretrained_model()

    # 2. Download real dataset
    download_dataset()

    # 3. Preprocess real records
    features, labels = load_and_preprocess_dataset(max_samples=5000)

    # 4. Stream real packets over network socket
    stream_packets_to_sentinel(features, labels)

    # 5. Query results
    query_results()

    print(f"\n{GREEN}Evaluation Complete. Inspect benchmark_results.csv for scientific paper figures.{NC}\n")

if __name__ == "__main__":
    main()