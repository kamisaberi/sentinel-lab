#!/usr/bin/env python3
"""
Sentinel-Lab: Wire-Speed Raw Socket Frame Blaster
Sends SLAB protocol binary frames (0x534C4142) directly to NIC driver rings via AF_PACKET/SOCK_RAW.
Now includes optional Grammar-Constrained Adversarial Evasion Mutator support.
"""

import sys
import os
import time
import socket
import struct
import json
import argparse
import numpy as np

from adversarial_mutator import AdversarialMutator

SLAB_MAGIC = 0x534C4142


class SlabSocketBlaster:
    def __init__(self, interface: str = "ens160", rate_limit_eps: int = 60000):
        self.interface = interface
        self.rate_limit_eps = rate_limit_eps
        self.sock = None
        self._init_socket()

    def _init_socket(self):
        try:
            # Bind directly to Linux AF_PACKET raw socket interface
            self.sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
            self.sock.bind((self.interface, 0))
            print(f"[+] [SlabBlaster] Bound RAW socket to interface: {self.interface}")
        except Exception as e:
            print(f"[!] [SlabBlaster Warning] Failed to bind raw socket on {self.interface}: {e}")
            print("    Falling back to loopback interface...")
            try:
                self.sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
                self.sock.bind(("lo", 0))
            except Exception as ex:
                print(f"[-] [SlabBlaster Error] Socket init failed: {ex}")

    def blast_dataset(self, features: np.ndarray, labels: np.ndarray):
        num_samples, dimensions = features.shape
        print(f"[+] [SlabBlaster] Starting blast: {num_samples} samples | {dimensions}D | Target Rate: {self.rate_limit_eps} EPS")

        period_ns = int(1e9 / self.rate_limit_eps) if self.rate_limit_eps > 0 else 0
        next_tx_time = time.perf_counter_ns()
        sent_count = 0

        for idx in range(num_samples):
            # 1. Pack 24-byte SLAB Header (=IQIII)
            header = struct.pack(
                "=IQIII",
                SLAB_MAGIC,
                idx + 1,            # Monotonic event ID
                int(labels[idx]),   # Ground truth label
                dimensions,         # Dynamic dimension D
                0                   # Flags
            )

            # 2. Serialize continuous float32 tensor payload
            payload = features[idx].astype(np.float32).tobytes()
            frame = header + payload

            # 3. Rate-limited transmission
            if period_ns > 0:
                now = time.perf_counter_ns()
                if now < next_tx_time:
                    time.sleep((next_tx_time - now) / 1e9)
                next_tx_time += period_ns

            if self.sock:
                try:
                    self.sock.send(frame)
                    sent_count += 1
                except Exception as err:
                    pass

        print(f"[+] [SlabBlaster] Completed transmission: {sent_count} frames blasted.")


def load_config_profile(config_path: str) -> dict:
    if os.path.exists(config_path):
        try:
            with open(config_path, "r") as f:
                data = json.load(f)
                return data.get("benchmark_profile", {})
        except Exception as e:
            print(f"[!] Error loading config profile {config_path}: {e}")
    return {}


def main():
    parser = argparse.ArgumentParser(description="Sentinel-Lab Raw Socket Frame Blaster")
    parser.add_argument("-i", "--interface", type=str, default=None, help="Network interface name")
    parser.add_argument("-r", "--rate-eps", type=int, default=None, help="Target injection rate in EPS")
    parser.add_argument("-c", "--config", type=str, default="configs/benchmark_profile.json", help="Path to benchmark_profile.json")
    parser.add_argument("-d", "--samples", type=int, default=1000, help="Synthetic test sample count")
    parser.add_argument("--evasion-mode", action="store_true", help="Apply grammar-constrained adversarial evasion noise")
    parser.add_argument("--epsilon", type=float, default=0.15, help="Epsilon bound for adversarial perturbation")
    args = parser.parse_args()

    profile = load_config_profile(args.config)

    interface = args.interface or profile.get("interface", "ens160")
    rate_eps = args.rate_eps or profile.get("target_rate_eps", 60000)

    # Generate synthetic feature tensor (1000 samples, 32 dimensions) for testing
    features = np.random.randn(args.samples, 32).astype(np.float32)
    labels = np.random.choice([0, 1], size=args.samples)

    # Apply grammar-constrained adversarial evasion if flag is set
    if args.evasion_mode:
        mutator = AdversarialMutator(config_path="configs/adversarial_profile.json", epsilon=args.epsilon)
        features = mutator.mutate_features(features, labels)

    blaster = SlabSocketBlaster(interface=interface, rate_limit_eps=rate_eps)
    blaster.blast_dataset(features, labels)


if __name__ == "__main__":
    main()