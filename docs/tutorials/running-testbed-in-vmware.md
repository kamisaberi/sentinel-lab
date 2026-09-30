---

### File: `sentinel-lab/docs/tutorials/running-testbed-in-vmware.md`

```markdown
# Executing the Research Harness Inside VMware Virtual Machines

Graduate students without access to physical multi-NIC bare-metal servers can execute `sentinel-lab` inside **VMware Workstation Pro, VMware Fusion, or VMware vSphere ESXi**.

---

## 1. Virtual Machine Hardware Prerequisites

Configure the virtual machine settings:
* **CPU:** 4 vCPUs with **"Virtualize Intel VT-x/EPT or AMD-V/RVI"** enabled.
* **RAM:** 8 GB RAM (100% Reserved, zero memory ballooning).
* **Virtual Adapter:** Set network adapter type to **`vmxnet3`**.
* **Virtual Disk:** NVMe Virtual Disk Controller.

---

## 2. Tuning VMware Virtual Interfaces for eBPF

By default, the Linux `vmxnet3` driver enables Large Receive Offload (LRO), which blocks native XDP hooks. Run these preparation commands inside the virtual machine before running the testbed:

```bash
# 1. Disable offloads that conflict with XDP
sudo ethtool -K ens33 lro off gro off rxvlan off txvlan off

# 2. Expand virtual receive queues
sudo ethtool -G ens33 rx 4096 tx 4096

# 3. Set standard MTU
sudo ip link set dev ens33 mtu 1500
```

---

## 3. Running the Testbed in Generic Mode

If your hypervisor does not support Native Driver mode on the virtual network adapter, pass `--xdp-mode SKB` to run in Generic XDP mode:

```bash
sudo python3 examples/run_full_evaluation.py \
    --interface ens33 \
    --xdp-mode SKB \
    --samples 10000
```

### Expected Output
```text
[*] Attached XDP filter to ens33 in Generic SKB mode.
[+] Baseline evaluation active: Latency p50: 2.45 µs | F1-Score: 0.9988
[+] Research harness verified inside virtualized VMware guest!
```
```

