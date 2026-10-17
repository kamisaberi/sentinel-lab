#!/usr/bin/env python3
"""
Sentinel-Lab: Post-Quantum Cryptography (PQC) OT Overhead Profiler Harness
Simulates NIST ML-KEM (Kyber) and ML-DSA (Dilithium) TLS 1.3 handshake byte expansions
and measures IP packet fragmentation and buffer bloat over MTU boundaries.
"""

import sys
import os
import json
import argparse


class PqcTunnelProfiler:
    def __init__(self, config_path: str = "configs/pqc_profile.json"):
        self.config_path = config_path
        self.kem_algorithm = "ML-KEM-768"
        self.sig_algorithm = "ML-DSA-3"
        self.mtu_bytes = 1500
        self.max_sla_ms = 1.0
        self._load_config()

    def _load_config(self):
        if os.path.exists(self.config_path):
            try:
                with open(self.config_path, "r") as f:
                    cfg = json.load(f).get("pqc_profile", {})
                    self.kem_algorithm = cfg.get("kdf_algorithm", "ML-KEM-768")
                    self.sig_algorithm = cfg.get("sig_algorithm", "ML-DSA-3")
                    self.mtu_bytes = int(cfg.get("mtu_bytes", 1500))
                    self.max_sla_ms = float(cfg.get("max_ot_sla_latency_ms", 1.0))
            except Exception as e:
                print(f"[!] [PqcProfiler Warning] Config read error: {e}")

    def profile_pqc_overhead(self, payload_bytes: int = 128):
        # NIST PQC Key / Signature Sizes (Bytes)
        pqc_sizes = {
            "ML-KEM-512": 1184 + 768,
            "ML-KEM-768": 1184 + 1088,
            "ML-KEM-1024": 1568 + 1568,
            "ML-DSA-2": 2420,
            "ML-DSA-3": 3293,
            "ML-DSA-5": 4595,
        }

        kem_overhead = pqc_sizes.get(self.kem_algorithm, 2272)
        sig_overhead = pqc_sizes.get(self.sig_algorithm, 3293)
        total_pqc_bytes = kem_overhead + sig_overhead
        total_frame_bytes = payload_bytes + total_pqc_bytes

        fragments = (total_frame_bytes + self.mtu_bytes - 1) // self.mtu_bytes
        latency_penalty_ms = 0.15 + (fragments * 0.18) + 0.35
        sla_breached = latency_penalty_ms > self.max_sla_ms

        print("==================================================================")
        print("     SENTINEL-LAB POST-QUANTUM CRYPTOGRAPHY OT PROFILER           ")
        print("==================================================================")
        print(f"[+] KEM Algorithm  : {self.kem_algorithm} ({kem_overhead} bytes)")
        print(f"[+] SIG Algorithm  : {self.sig_algorithm} ({sig_overhead} bytes)")
        print(f"[+] Total PQC Size : {total_pqc_bytes} bytes")
        print(f"[+] MTU Limit      : {self.mtu_bytes} bytes")
        print(f"[+] IP Fragments   : {fragments} fragments / packet")
        print(f"[+] Total Latency  : {latency_penalty_ms:.2f} ms (SLA Limit: {self.max_sla_ms:.2f} ms)")
        print(f"[+] SLA Status     : {'[PASSED] SLA Preserved' if not sla_breached else '[BREACH] PQC Latency Breach'}")
        print("==================================================================\n")


def main():
    parser = argparse.ArgumentParser(description="Sentinel-Lab PQC OT Overhead Profiler")
    parser.add_argument("-c", "--config", type=str, default="configs/pqc_profile.json", help="Path to pqc_profile.json")
    parser.add_argument("-p", "--payload", type=int, default=128, help="Raw SCADA payload size")
    args = parser.parse_args()

    profiler = PqcTunnelProfiler(config_path=args.config)
    profiler.profile_pqc_overhead(payload_bytes=args.payload)


if __name__ == "__main__":
    main()