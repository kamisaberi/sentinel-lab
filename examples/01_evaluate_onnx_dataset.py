#!/usr/bin/env python3
"""
Universal Evaluation Harness:
Evaluates any ONNX model and dataset against the invariant Sentinel-Lab C++ daemon.
Requires ZERO C++ code modifications.
"""

import os
import sys
import time
import socket
import struct
import urllib.request
import numpy as np

# Configuration
TARGET_IP = "127.0.0.1"
TARGET_PORT = 9000
ONNX_MODEL_PATH = "models/network_threat.onnx"
PRETRAINED_MODEL_URL = "https://huggingface.co/darkknight25/ddos_xgboost_onnx/resolve/main/ddos_detection_model.onnx"

def ensure_model():
    os.makedirs("models", exist_ok=True)
    if os.path.exists(ONNX_MODEL_PATH):
        print(f"[Model Check] Found active ONNX model: {ONNX_MODEL_PATH}")
        return
    print(f"[Model Check] Downloading pre-trained ONNX model from Hugging Face...")
    try:
        urllib.request.urlretrieve(PRETRAINED_MODEL_URL, ONNX_MODEL_PATH)
        print(f"[Model Check] Downloaded to {ONNX_MODEL_PATH} ({os.path.getsize(ONNX_MODEL_PATH)} bytes)")
    except Exception as e:
        print(f"[Model Check Error] Download failed: {e}")

def main():
    ensure_model()

    num_samples = 5000
    num_features = 32  # Can be changed to 42, 80, etc. without modifying C++

    print(f"\n[Dataset Harness] Generating {num_samples} test records with {num_features} features...")
    np.random.seed(42)

    # 1. Generate Benign Normal Samples (Label = 0)
    num_benign = int(num_samples * 0.85)
    benign_data = np.random.normal(loc=0.30, scale=0.08, size=(num_benign, num_features)).astype(np.float32)
    benign_labels = np.zeros(num_benign, dtype=np.int32)

    # 2. Generate Malicious Exploit Samples (Label = 1)
    num_attack = num_samples - num_benign
    attack_data = np.random.normal(loc=0.92, scale=0.04, size=(num_attack, num_features)).astype(np.float32)
    attack_labels = np.ones(num_attack, dtype=np.int32)

    features = np.clip(np.vstack([benign_data, attack_data]), 0.0, 1.0)
    labels = np.concatenate([benign_labels, attack_labels])

    # Shuffle
    idx = np.random.permutation(num_samples)
    features, labels = features[idx], labels[idx]

    # Open network socket to Sentinel-Lab
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print(f"[Dataset Harness] Streaming {num_samples} packets to Sentinel-Lab ({TARGET_IP}:{TARGET_PORT})...")

    t0 = time.time()
    for i in range(num_samples):
        # Format: Magic(4s) | EventID(Q) | GroundTruth(i) | NumFeatures(i) | Floats(bytes)
        payload = struct.pack("!4sQii", b"SLAB", i + 1, int(labels[i]), num_features) + features[i].tobytes()
        sock.sendto(payload, (TARGET_IP, TARGET_PORT))

        if (i + 1) % 100 == 0:
            time.sleep(0.001)

    duration = time.time() - t0
    rate = num_samples / duration if duration > 0 else 0
    print(f"[Dataset Harness] Stream finished: {num_samples} packets sent in {duration:.2f}s ({rate:.2f} EPS).")

if __name__ == "__main__":
    main()