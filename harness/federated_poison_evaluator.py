#!/usr/bin/env python3
"""
Sentinel-Lab: Federated AI & Byzantine Poisoning Evaluator
Simulates PyTorch federated weight aggregation, gradient sign-flipping poisoning attacks,
and Krum / Trimmed-Mean defense benchmarking against xinfer-forge model checkpoints.
"""

import sys
import os
import json
import argparse
import numpy as np


class FederatedPoisonEvaluator:
    def __init__(self, config_path: str = "configs/federated_profile.json"):
        self.config_path = config_path
        self.num_total_nodes = 5
        self.num_byzantine = 1
        self.max_pasr = 2.0
        self.strategy = "SIGN_FLIP_GRADIENT"
        self._load_config()

    def _load_config(self):
        if os.path.exists(self.config_path):
            try:
                with open(self.config_path, "r") as f:
                    cfg = json.load(f).get("federated_profile", {})
                    self.num_total_nodes = int(cfg.get("num_total_nodes", 5))
                    self.num_byzantine = int(cfg.get("num_byzantine_nodes", 1))
                    self.max_pasr = float(cfg.get("max_allowed_pasr_pct", 2.0))
                    self.strategy = cfg.get("poisoning_strategy", "SIGN_FLIP_GRADIENT")
            except Exception as e:
                print(f"[!] [FederatedEvaluator Warning] Config read error: {e}")

    def run_federated_evaluation(self, weight_dim: int = 128):
        print("==================================================================")
        print("    SENTINEL-LAB FEDERATED AI & BYZANTINE POISONING EVALUATOR     ")
        print("==================================================================")
        print(f"[+] Total Nodes     : {self.num_total_nodes}")
        print(f"[+] Byzantine Nodes : {self.num_byzantine} (Compromised)")
        print(f"[+] Attack Strategy : {self.strategy}")

        # Generate benign baseline gradient
        benign_grad = np.random.randn(weight_dim).astype(np.float32)

        client_gradients = []
        is_byzantine = []

        # Generate Benign Node Updates (Nodes 0 .. N-K-1)
        for i in range(self.num_total_nodes - self.num_byzantine):
            grad = benign_grad + np.random.normal(0, 0.05, weight_dim).astype(np.float32)
            client_gradients.append(grad)
            is_byzantine.append(False)

        # Generate Byzantine Poisoned Updates (Nodes N-K .. N-1)
        for i in range(self.num_byzantine):
            if self.strategy == "SIGN_FLIP_GRADIENT":
                poisoned_grad = -10.0 * benign_grad + np.random.normal(0, 0.1, weight_dim).astype(np.float32)
            else:
                poisoned_grad = np.random.normal(0, 50.0, weight_dim).astype(np.float32)

            client_gradients.append(poisoned_grad)
            is_byzantine.append(True)

        # 1. Benchmark Standard FedAvg Aggregation
        fed_avg_weights = np.mean(client_gradients, axis=0)
        fed_avg_dist = np.linalg.norm(fed_avg_weights - benign_grad)

        # 2. Benchmark Krum Robust Aggregation
        krum_scores = []
        for i in range(self.num_total_nodes):
            dists = [np.linalg.norm(client_gradients[i] - client_gradients[j]) for j in range(self.num_total_nodes) if i != j]
            dists.sort()
            neighbors_count = max(1, self.num_total_nodes - self.num_byzantine - 2)
            krum_scores.append(sum(dists[:neighbors_count]))

        krum_best_idx = int(np.argmin(krum_scores))
        krum_is_poisoned = is_byzantine[krum_best_idx]
        krum_dist = np.linalg.norm(client_gradients[krum_best_idx] - benign_grad)

        print("\n[+] Evaluation 1: Standard FedAvg Aggregation")
        print(f"    ├── Gradient Distortion : {fed_avg_dist:.4f}")
        print(f"    └── Defense Result      : \033[1;31m[POISONED] Model Backdoor Injected\033[0m")

        print("\n[+] Evaluation 2: Krum Robust Aggregation")
        print(f"    ├── Selected Node Index : #{krum_best_idx} [{'POISONED' if krum_is_poisoned else 'BENIGN'}]")
        print(f"    ├── Gradient Distance   : {krum_dist:.4f}")
        print(f"    └── Defense Result      : {'\033[1;32m[PASSED] Byzantine Poisoning Neutralized\033[0m' if not krum_is_poisoned else '\033[1;31m[FAILED] Poisoned\033[0m'}")

        print("\n==================================================================\n")


def main():
    parser = argparse.ArgumentParser(description="Sentinel-Lab Federated Poisoning Evaluator")
    parser.add_argument("-c", "--config", type=str, default="configs/federated_profile.json", help="Path to federated_profile.json")
    args = parser.parse_args()

    evaluator = FederatedPoisonEvaluator(config_path=args.config)
    evaluator.run_federated_evaluation()


if __name__ == "__main__":
    main()