# Raw Socket Capabilities & Execution Boundaries (`CAP_NET_RAW`)

Streaming SLAB frames directly onto network interfaces requires binding to Linux raw packet sockets (`AF_PACKET`, `SOCK_RAW`). Unprivileged users will encounter operational permission faults.

---

## 1. Symptom

```text
PermissionError: [Errno 1] Operation not permitted
[FATAL] socket(AF_PACKET, SOCK_RAW) failed: EPERM (Operation not permitted)
```

---

## 2. Resolution Strategies

### Option A: Execute with Elevated Superuser Privileges
Run the evaluation script via `sudo`:

```bash
sudo python3 examples/run_full_evaluation.py
```

### Option B: Assign POSIX Capabilities to the Python Runtime
If executing in an unprivileged multi-user lab environment where `sudo` is restricted, grant the `CAP_NET_RAW` and `CAP_NET_ADMIN` capabilities directly to the Python interpreter:

```bash
# Locate your active Python binary or virtual environment path
PYTHON_BIN=$(which python3)

# Grant raw socket and network administration capabilities
sudo setcap 'cap_net_raw,cap_net_admin=+ep' "${PYTHON_BIN}"
```

Verify that the capabilities are bound:

```bash
getcap "${PYTHON_BIN}"
# Expected Output: /usr/bin/python3.12 = cap_net_admin,cap_net_raw+ep
```

---

## 3. Container & Docker Considerations

When running the harness inside a Docker container, pass `--cap-add=NET_RAW` and `--cap-add=NET_ADMIN`:

```bash
docker run --rm -it \
    --cap-add=NET_RAW \
    --cap-add=NET_ADMIN \
    --network=host \
    sentinel-lab:latest
```

