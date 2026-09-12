#!/usr/bin/env bash
set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

TARGET_IP="172.30.0.1"

echo -e "${GREEN}=====================================================${NC}"
echo -e "${GREEN} Sentinel-Lab Simulation Attack Verification         ${NC}"
echo -e "${GREEN}=====================================================${NC}"

# 1. Forward test Syslog event
echo -e "\n${YELLOW}[Step 1] Emulating Syslog stream from client server...${NC}"
sudo docker exec sim-ubuntu-server logger "TEST_EVENT: Failed password for root from 172.30.0.250 port 54321 ssh2" || true
echo -e "${GREEN}Syslog event sent!${NC}"

# 2. Port scan attack
echo -e "\n${RED}[Step 2] Attacker (172.30.0.250) launching Nmap Port Scan...${NC}"
sudo docker exec sim-attacker nmap -sS -p 22,80,443,502,8443 ${TARGET_IP} || true

# 3. Unauthorized Modbus write
echo -e "\n${RED}[Step 3] Attacker launching unauthorized Modbus SCADA write...${NC}"
sudo docker exec sim-attacker sh -c "echo 'MALICIOUS_MODBUS_WRITE' | nc -w 1 ${TARGET_IP} 502" || true

# 4. Check REST API
echo -e "\n${GREEN}[Step 4] Checking Sentinel-Lab Telemetry API...${NC}"
if curl -s http://localhost:8443 | grep -q "total_events"; then
    echo -e "${GREEN}Sentinel-Lab API is ACTIVE!${NC}"
    curl -s http://localhost:8443
    echo ""
else
    echo -e "${YELLOW}Sentinel-Lab daemon not running on port 8443. Start with 'sudo ./sentinel_lab'${NC}"
fi

echo -e "\n${GREEN}=====================================================${NC}"
echo -e "${GREEN} Attack Verification Complete.                      ${NC}"
echo -e "${GREEN}=====================================================${NC}"