#!/usr/bin/env python3
import sys
import os
import pandas as pd
import numpy as np

def main(csv_path="benchmark_results.csv"):
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found. Run sentinel_lab first!")
        sys.exit(1)

    df = pd.read_csv(csv_path)
    print("==================================================================")
    print("          SENTINEL-LAB SCIENTIFIC EVALUATION SUMMARY              ")
    print("==================================================================")
    print(f"Total Evaluated Samples : {len(df)}")
    
    # Latencies
    latencies = df['Latency_us']
    print(f"Mean Latency (us)       : {latencies.mean():.2f}")
    print(f"P50 Median Latency (us) : {latencies.quantile(0.50):.2f}")
    print(f"P95 Latency (us)        : {latencies.quantile(0.95):.2f}")
    print(f"P99 Latency (us)        : {latencies.quantile(0.99):.2f}")

    # Confusion Matrix
    y_true = df['GroundTruth']
    # Action 2 = KernelDropped, Action 1 = Logged, Action 0 = Passed
    y_pred = (df['Action'] == 2).astype(int)

    tp = np.sum((y_true == 1) & (y_pred == 1))
    fp = np.sum((y_true == 0) & (y_pred == 1))
    tn = np.sum((y_true == 0) & (y_pred == 0))
    fn = np.sum((y_true == 1) & (y_pred == 0))

    accuracy = (tp + tn) / len(y_true)
    precision = tp / (tp + fp) if (tp + fp) > 0 else 1.0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 1.0
    f1 = 2 * (precision * recall) / (precision + recall) if (precision + recall) > 0 else 0.0

    print("------------------------------------------------------------------")
    print(f"Accuracy                : {accuracy * 100:.2f}%")
    print(f"Precision               : {precision * 100:.2f}%")
    print(f"Recall                  : {recall * 100:.2f}%")
    print(f"F1-Score                : {f1 * 100:.2f}%")
    print("==================================================================")

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "benchmark_results.csv"
    main(path)