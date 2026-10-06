# Wire-Speed Packet Injection via Linux Raw Sockets (`AF_PACKET`)

To evaluate driver-level mitigation realistically, `sentinel-lab` transmits SLAB binary frames over native Linux raw packet sockets (**`AF_PACKET` / `SOCK_RAW`**), sustaining transmission rates exceeding **$60{,}000\text{ packets/second}$** from user space.

---

## 1. Raw Socket Transmission Architecture

```text
 [ Python Dataset Ingestion Loop (socket_injector.py) ]
                          │
                          ▼ struct.pack(SLAB_HEADER + FLOAT_TENSOR)
 ┌─────────────────────────────────────────────────────────────┐
 │ Raw Packet Socket: socket(AF_PACKET, SOCK_RAW)              │
 │  - Bypasses TCP/UDP protocol layers                         │
 │  - Transmits raw Ethernet frames directly to NIC driver ring │
 └────────────────────────┬────────────────────────────────────┘
                          │
                          ▼ Direct Device Injection (eth0 / lo)
 ┌─────────────────────────────────────────────────────────────┐
 │ Linux Kernel Ingress: Native XDP Hook (xdp_filter.o)        │
 │  - Packets evaluated by driver before socket allocations    │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Raw Socket Blaster Implementation (`harness/socket_injector.py`)

```python
import socket
import struct
import time
import numpy as np
from typing import List

SLAB_MAGIC = 0x534C4142 # "SLAB"

class SlabSocketBlaster:
    def __init__(self, interface: str = "lo"):
        self.interface = interface
        # Open raw Ethernet frame socket
        self.sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
        self.sock.bind((interface, 0))

    def blast_dataset(self, features: np.ndarray, labels: np.ndarray, rate_limit_eps: int = 60000):
        num_samples = len(features)
        target_delay = 1.0 / rate_limit_eps if rate_limit_eps > 0 else 0.0
        
        print(f"[*] Blasting {num_samples} SLAB frames over interface {self.interface}...")
        start_time = time.perf_counter()

        for idx in range(num_samples):
            # 1. Pack 24-byte SLAB Header
            header = struct.pack(
                "=IQII I",
                SLAB_MAGIC,
                idx + 1,
                int(labels[idx]),
                32, # 32 dimensions
                0   # Flags
            )

            # 2. Serialize 32-dim Float Tensor Payload
            payload = features[idx].tobytes()
            packet = header + payload

            # 3. Transmit Frame Directly to Driver Layer
            self.sock.send(packet)

            if target_delay > 0:
                time.sleep(target_delay)

        elapsed = time.perf_counter() - start_time
        actual_eps = num_samples / elapsed
        print(f"[+] Injection complete: {num_samples} frames in {elapsed:.2f}s ({actual_eps:.1f} EPS)")
```

