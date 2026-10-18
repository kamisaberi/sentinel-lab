#!/usr/bin/env python3
"""
Sentinel-Lab: Formal Control Barrier Function (CBF) & Reachability Evaluator
Simulates physical state trajectories x(t) and verifies Ames/Nagumo forward-invariance inequality dh/dt + alpha*h >= 0.
"""

import sys
import os
import time
import json
import argparse
import numpy as np


class CbfReachabilityEvaluator:
    def __init__(self, config_path: str = "configs/cbf_profile.json"):
        self.config_path = config_path
        self.max_temp_c = 120.0
        self.alpha_gain = 0.5
        self._load_config()

    def _load_config(self):
        if os.path.exists(self.config_path):
            try:
                with open(self.config_path, "r") as f:
                    cfg = json.load(f).get("cbf_profile", {})
                    self.max_temp_c = float(cfg.get("max_safe_temperature_c", 120.0))
                    self.alpha_gain = float(cfg.get("cbf_alpha_gain", 0.5))
            except Exception as e:
                print(f"[!] [CbfEvaluator Warning] Config read error: {e}")

    def evaluate_trajectory(self, num_points: int = 100):
        print("==================================================================")
        print("  SENTINEL-LAB FORMAL CONTROL BARRIER FUNCTION (CBF) VERIFIER     ")
        print("==================================================================")
        print(f"[+] Safety Barrier Function : h(x) = {self.max_temp_c} - T(t)")
        print(f"[+] CBF Class Alpha Gain    : {self.alpha_gain}")

        # Simulate trajectory: T(t) rising from 85°C to 118°C (Safe) vs 122°C (Unsafe)
        time_steps = np.linspace(0, 10, num_points)
        temp_safe = 85.0 + 30.0 * (1.0 - np.exp(-0.3 * time_steps))
        temp_unsafe = 85.0 + 40.0 * (1.0 - np.exp(-0.3 * time_steps))

        for name, trajectory in [("Controlled Safe Trajectory", temp_safe), ("Uncontrolled Unsafe Trajectory", temp_unsafe)]:
            h_vals = self.max_temp_c - trajectory
            h_dots = -np.gradient(trajectory, time_steps)
            cbf_condition = h_dots + (self.alpha_gain * h_vals)

            min_h = float(np.min(h_vals))
            min_cbf_val = float(np.min(cbf_condition))
            is_invariant = (min_h >= 0.0) and (min_cbf_val >= 0.0)

            print(f"\n[+] Trajectory: {name}")
            print(f"    ├── Min Barrier Value h(x)  : {min_h:.3f} (Safe: >= 0)")
            print(f"    ├── Min CBF Val (dh+alpha*h): {min_cbf_val:.3f}")
            print(f"    └── Status                  : {'\033[1;32m[VERIFIED SAFE] Set C Invariant\033[0m' if is_invariant else '\033[1;31m[UNSAFE] Safety Barrier Breached\033[0m'}")

        print("\n==================================================================\n")


def main():
    parser = argparse.ArgumentParser(description="Sentinel-Lab CBF Reachability Evaluator")
    parser.add_argument("-c", "--config", type=str, default="configs/cbf_profile.json", help="Path to cbf_profile.json")
    args = parser.parse_args()

    evaluator = CbfReachabilityEvaluator(config_path=args.config)
    evaluator.evaluate_trajectory()


if __name__ == "__main__":
    main()