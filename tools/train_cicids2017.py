#!/usr/bin/env python3
"""Downloads real CIC-IDS2017 dataset, trains an MLP classifier, and exports models/network_threat.onnx"""

import os
import requests
import pandas as pd
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from tqdm import tqdm

DATASET_URL = "https://huggingface.co/datasets/c01dsnap/CIC-IDS2017/resolve/main/Friday-WorkingHours-Afternoon-PortScan.pcap_ISCX.csv"
DATASET_CSV = "tools/cicids2017_portscan.csv"
MODEL_OUTPUT = "models/network_threat.onnx"

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

class NetworkThreatClassifier(nn.Module):
    def __init__(self, input_dim=32):
        super(NetworkThreatClassifier, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, 64),
            nn.BatchNorm1d(64),
            nn.ReLU(),
            nn.Dropout(0.2),
            nn.Linear(64, 32),
            nn.ReLU(),
            nn.Linear(32, 16),
            nn.ReLU(),
            nn.Linear(16, 2),
            nn.Softmax(dim=1)
        )

    def forward(self, x):
        return self.net(x)

def download():
    if os.path.exists(DATASET_CSV):
        print(f"[CIC-IDS2017] Using cached dataset: {DATASET_CSV}")
        return
    print("[CIC-IDS2017] Downloading real CIC-IDS2017 dataset from Hugging Face...")
    resp = requests.get(DATASET_URL, stream=True)
    total = int(resp.headers.get('content-length', 0))
    with open(DATASET_CSV, 'wb') as f, tqdm(total=total, unit='B', unit_scale=True) as bar:
        for chunk in resp.iter_content(chunk_size=1024 * 1024):
            f.write(chunk)
            bar.update(len(chunk))

def main():
    os.makedirs("models", exist_ok=True)
    download()

    df = pd.read_csv(DATASET_CSV, low_memory=False)
    df.columns = df.columns.str.strip()
    df.replace([np.inf, -np.inf], np.nan, inplace=True)
    df.dropna(subset=FEATURE_COLS + ['Label'], inplace=True)

    labels = (df['Label'] != 'BENIGN').astype(int).values
    features = df[FEATURE_COLS].astype(np.float32).values

    scaler = StandardScaler()
    features = scaler.fit_transform(features)

    X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size=0.2, random_state=42)

    model = NetworkThreatClassifier(input_dim=32)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=0.002)

    dataset = torch.utils.data.TensorDataset(torch.tensor(X_train, dtype=torch.float32), torch.tensor(y_train, dtype=torch.long))
    loader = torch.utils.data.DataLoader(dataset, batch_size=2048, shuffle=True)

    model.train()
    for epoch in range(5):
        loss_val = 0.0
        for bx, by in loader:
            optimizer.zero_grad()
            out = model(bx)
            l = criterion(out, by)
            l.backward()
            optimizer.step()
            loss_val += l.item()
        print(f"Epoch [{epoch+1}/5] - Loss: {loss_val/len(loader):.5f}")

    model.eval()
    dummy = torch.randn(1, 32, dtype=torch.float32)
    torch.onnx.export(
        model, dummy, MODEL_OUTPUT,
        export_params=True, opset_version=17, do_constant_folding=True,
        input_names=['input'], output_names=['scores'],
        dynamic_axes={'input': {0: 'batch_size'}, 'scores': {0: 'batch_size'}}
    )
    print(f"[SUCCESS] Exported: {MODEL_OUTPUT} ({os.path.getsize(MODEL_OUTPUT)} bytes)")

if __name__ == "__main__":
    main()