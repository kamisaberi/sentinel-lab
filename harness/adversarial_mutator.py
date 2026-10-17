#!/usr/bin/env python3
"""
Sentinel-Lab: Grammar-Constrained Adversarial Feature Mutator
Applies bounded epsilon perturbations (L-infinity / L2) to continuous SLAB feature tensors
while strictly preserving protocol syntax and header integrity.
"""

import os
import json
import numpy as np


class AdversarialMutator:
    def __init__(self, config_path: str = "configs/adversarial_profile.json", epsilon: float = 0.15):
        self.config_path = config_path
        self.epsilon = epsilon
        self.p_norm = "L_INF"
        self.immutable_features = [0, 1, 2]  # Preserve protocol header & function code indices
        self.evasion_strategy = "JITTER_AND_PAYLOAD_DELTA"
        self._load_config()

    def _load_config(self):
        if os.path.exists(self.config_path):
            try:
                with open(self.config_path, "r") as f:
                    cfg = json.load(f).get("adversarial_profile", {})
                    self.epsilon = float(cfg.get("epsilon_bound", self.epsilon))
                    self.p_norm = cfg.get("p_norm", "L_INF")
                    self.immutable_features = cfg.get("immutable_features", [0, 1, 2])
                    self.evasion_strategy = cfg.get("evasion_strategy", "JITTER_AND_PAYLOAD_DELTA")
            except Exception as e:
                print(f"[!] [AdversarialMutator] Warning reading {self.config_path}: {e}")

    def mutate_features(self, features: np.ndarray, labels: np.ndarray) -> np.ndarray:
        """
        Applies grammar-constrained adversarial noise only to attack samples (label == 1),
        leaving benign traffic untouched and preserving protocol header fields.
        """
        mutated = features.copy().astype(np.float32)
        num_samples, num_features = mutated.shape

        for i in range(num_samples):
            # Only perturb attack frames (label == 1) for evasion stress-testing
            if labels[i] == 1:
                if self.p_norm == "L_INF":
                    # Uniform bounded noise in [-epsilon, +epsilon]
                    noise = np.random.uniform(-self.epsilon, self.epsilon, size=num_features).astype(np.float32)
                else:
                    # L2 norm bounded noise
                    noise = np.random.randn(num_features).astype(np.float32)
                    norm = np.linalg.norm(noise)
                    if norm > 0:
                        noise = (noise / norm) * self.epsilon

                # Zero out perturbations on immutable protocol header slots
                for idx in self.immutable_features:
                    if idx < num_features:
                        noise[idx] = 0.0

                mutated[i] += noise

        print(f"[+] [AdversarialMutator] Applied {self.evasion_strategy} (eps={self.epsilon:.3f}, norm={self.p_norm}) to attack vectors.")
        return mutated