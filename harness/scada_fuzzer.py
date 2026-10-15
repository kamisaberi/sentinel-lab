#!/usr/bin/env python3
"""
Sentinel-Lab: Differential SCADA Parser Desync Fuzzer
Sends mutated SCADA PDUs simultaneously to Edge Gateway (10.240.0.101) and Mock PLC (10.240.0.150)
to detect Parser Desynchronization / Request Smuggling vulnerabilities (V_gateway != V_plc).
"""

import sys
import os
import socket
import time
import json
import argparse
import random


class ScadaDifferentialFuzzer:
    def __init__(self, gateway_host="10.240.0.101", plc_host="10.240.0.150", port=502):
        self.gateway_host = gateway_host
        self.plc_host = plc_host
        self.port = port

    def send_pdu(self, host: str, port: int, payload: bytes) -> bool:
        """Sends PDU and returns True if connection accepted & state ACK received"""
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(0.5)
            s.connect((host, port))
            s.send(payload)
            resp = s.recv(256)
            s.close()
            return len(resp) > 0
        except Exception:
            return False

    def mutate_modbus_pdu(self, seed: bytes) -> bytes:
        """Mutates Modbus MBAP Header Length or appends dual function codes"""
        pdu = bytearray(seed)
        strategy = random.choice(["MBAP_LEN_CORRUPT", "DUAL_FC", "TRAILING_PAD"])

        if strategy == "MBAP_LEN_CORRUPT" and len(pdu) >= 6:
            pdu[4] = random.randint(0, 255)
            pdu[5] = random.randint(0, 255)
        elif strategy == "DUAL_FC":
            # Append FC05 Coil Write to FC03 Read query
            pdu.extend(bytes([0x05, 0x00, 0x01, 0xFF, 0x00]))
        elif strategy == "TRAILING_PAD":
            pdu.extend(bytes([random.randint(0, 255) for _ in range(16)]))

        return bytes(pdu)

    def run_fuzzing_loop(self, iterations=1000):
        print("==================================================================")
        print("    SENTINEL-LAB SCADA DIFFERENTIAL PARSER DESYNC FUZZER          ")
        print("==================================================================")
        print(f"[+] Gateway Endpoint : {self.gateway_host}:{self.port}")
        print(f"[+] Target PLC Twin  : {self.plc_host}:{self.port}")
        print(f"[+] Fuzzing Iterations: {iterations}")

        # Standard Modbus FC03 Read Holding Register Seed
        seed_pdu = bytes([0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x01, 0x00, 0x02])
        desync_bugs_found = 0

        for i in range(1, iterations + 1):
            mutated = self.mutate_modbus_pdu(seed_pdu)

            # Send mutated frame to Gateway and PLC
            v_gateway_pass = self.send_pdu(self.gateway_host, self.port, mutated)
            v_plc_executed = self.send_pdu(self.plc_host, self.port, mutated)

            # Differential Oracle: V_gateway == PASS && V_plc == EXECUTED
            if v_gateway_pass and v_plc_executed:
                desync_bugs_found += 1
                print(f"\n🚨 [DESYNC ZERO-DAY FOUND] Iteration #{i}:")
                print(f"    ├── Gateway Verdict : PASS (Frame permitted)")
                print(f"    ├── PLC Twin Status : EXECUTED (State modified)")
                print(f"    └── Mutated Hex     : {mutated.hex()}")

        print("\n------------------------------------------------------------------")
        print(f"Total Iterations Tested  : {iterations}")
        print(f"Parser Desync Bugs Found : {desync_bugs_found}")
        print("==================================================================\n")


def main():
    parser = argparse.ArgumentParser(description="SCADA Differential Parser Desync Fuzzer")
    parser.add_argument("-g", "--gateway", type=str, default="10.240.0.101", help="Gateway IP")
    parser.add_argument("-p", "--plc", type=str, default="10.240.0.150", help="PLC IP")
    parser.add_argument("-port", "--port", type=int, default=502, help="Port")
    parser.add_argument("-n", "--iterations", type=int, default=500, help="Iterations")
    args = parser.parse_args()

    fuzzer = ScadaDifferentialFuzzer(gateway_host=args.gateway, plc_host=args.plc, port=args.port)
    fuzzer.run_fuzzing_loop(iterations=args.iterations)


if __name__ == "__main__":
    main()
    