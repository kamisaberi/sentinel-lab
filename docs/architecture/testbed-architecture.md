# Decoupled C++20 Research Engine Architecture & Polling Loop

`sentinel-lab` is engineered as an empirical testbed daemon (`sentinel_lab`) written in ISO C++20. It decouples high-speed raw socket packet reception from neural network evaluation and metric recording, ensuring that hardware benchmarks reflect real-world pipeline throughput.

---

## 1. Internal Engine Subsystems

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ Raw Network Ingress: Linux AF_PACKET / AF_XDP Driver Hook   │
 └──────────────────────────────┬──────────────────────────────┘
                                │ Zero-Copy Buffer Descriptor
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. Ingestion Worker Loop (Non-Blocking Epoll / Busy-Poll)   │
 │   - Validates SLAB Magic Header: 0x534C4142                 │
 │   - Memory-maps raw payload pointer directly to Tensor view │
 └──────────────────────────────┬──────────────────────────────┘
                                │ Passes std::span<const float>
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 2. Silicon Evaluation Engine (libxinfer.so Integration)     │
 │   - Dispatches forward pass to Intel NPU or NVIDIA GPU      │
 │   - Captures exact hardware execution cycle counts          │
 └──────────────────────────────┬──────────────────────────────┘
                                │
        ┌───────────────────────┴───────────────────────┐
        ▼ Ground Truth Parity Check                     ▼ Hardware Timestamping
 ┌─────────────────────────────┐         ┌─────────────────────────────┐
 │ 3. Scientific Metric Engine │         │ 4. Latency Distribution Bin │
 │  - Increments Confusion Mat │         │  - High-Resolution Latency  │
 │  - Real-Time F1 / Accuracy  │         │    Histogram (Nanosecond)   │
 └─────────────────────────────┘         └─────────────────────────────┘
```

---

## 2. Low-Latency Socket Polling Loop (`src/engine/polling_loop.cpp`)

To avoid OS scheduler latency, `sentinel_lab` operates a dedicated busy-polling thread pinned to an isolated CPU core:

```cpp
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sentinel_lab/slab_protocol.hpp>
#include <sentinel_lab/metrics.hpp>
#include <xinfer/xinfer.hpp>

namespace sentinel::lab {

void run_benchmark_polling_loop(
    int raw_sock_fd, 
    xinfer::InferenceEngine& ai_engine, 
    ScientificMetrics& metrics,
    std::atomic<bool>& keep_running
) {
    alignas(64) uint8_t packet_buffer[2048];

    while (keep_running.load(std::memory_order_relaxed)) {
        // 1. Non-blocking packet reception from raw socket
        ssize_t bytes_received = ::recv(raw_sock_fd, packet_buffer, sizeof(packet_buffer), MSG_DONTWAIT);
        if (bytes_received <= 0) {
            continue; // Busy-poll: zero thread sleep to avoid scheduler jitter
        }

        // 2. Hardware Timer: Ingress Mark
        uint64_t t_ingress = read_invariant_tsc();

        // 3. Zero-Copy Header Cast
        if (bytes_received < sizeof(SlabHeader)) continue;
        const auto* slab = reinterpret_cast<const SlabHeader*>(packet_buffer);

        if (slab->magic != SLAB_MAGIC) continue; // Skip non-SLAB frames

        // 4. Map continuous float tensor view (Skipping 24-byte SLAB header)
        const float* tensor_data = reinterpret_cast<const float*>(packet_buffer + sizeof(SlabHeader));
        std::span<const float> input_features(tensor_data, slab->dimensions);

        // 5. Execute Neural Forward Pass
        auto input_tensor = ai_engine.get_input_tensor(0);
        input_tensor->copy_from_host(input_features.data(), input_features.size_bytes());
        ai_engine.forward();

        // 6. Hardware Timer: Egress / Mitigation Mark
        uint64_t t_mitigated = read_invariant_tsc();
        uint64_t delta_cycles = t_mitigated - t_ingress;

        // 7. Update Classification & Latency Metrics
        float prediction = *ai_engine.get_output_tensor(0)->data<float>();
        metrics.record_event(slab->ground_truth, prediction > 0.5f, delta_cycles);
    }
}

} // namespace sentinel::lab
```

