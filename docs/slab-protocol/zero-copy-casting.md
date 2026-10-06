# Memory-Mapping & Zero-Copy Casting

Conventional network parsers deserialize incoming byte arrays by allocating new heap objects and copying fields element-by-element. This overhead degrades ingestion throughput.

`sentinel-lab` eliminates copies through **Direct Pointer Casting via `std::span`**.

---

## 1. Zero-Copy Ingestion Flow

```text
 [ Linux Raw Socket Ingress Buffer: packet_buffer ]
                     │
                     ▼ Direct In-Place Memory Map
 ┌─────────────────────────────────────────────────────────────┐
 │ const auto* slab = reinterpret_cast<const SlabHeader*>(buf) │
 └───────────────────┬─────────────────────────────────────────┘
                     │ (Validate magic == 0x534C4142)
                     ▼ Pointer Offset (+24 Bytes)
 ┌─────────────────────────────────────────────────────────────┐
 │ const float* raw_tensor = reinterpret_cast<const float*>(   │
 │                                   buf + sizeof(SlabHeader)) │
 │ std::span<const float> tensor_view(raw_tensor, slab->dim)   │
 └───────────────────┬─────────────────────────────────────────┘
                     │
                     ▼ Passed Directly to AI Silicon
 [ Hardware Memory Pointer: No intermediate allocations ]
```

---

## 2. In-Engine Implementation (`src/engine/packet_consumer.cpp`)

```cpp
#include <sentinel_lab/slab_protocol.hpp>
#include <xinfer/tensor.hpp>
#include <span>

namespace sentinel::lab {

bool process_slab_packet_zerocopy(
    std::span<const uint8_t> raw_network_frame,
    std::shared_ptr<xinfer::Tensor>& target_tensor,
    uint32_t& out_ground_truth
) {
    // 1. Boundary Check: Header must fit within incoming packet
    if (raw_network_frame.size() < sizeof(SlabHeader)) {
        return false;
    }

    // 2. Cast Header Pointer
    const auto* header = reinterpret_cast<const SlabHeader*>(raw_network_frame.data());
    if (header->magic != SLAB_MAGIC) {
        return false; // Non-SLAB packet
    }

    // 3. Boundary Check: Verify payload contains claimed float dimensions
    size_t expected_payload_bytes = header->dimensions * sizeof(float);
    if (raw_network_frame.size() < sizeof(SlabHeader) + expected_payload_bytes) {
        return false; // Truncated packet
    }

    out_ground_truth = header->ground_truth;

    // 4. Zero-Copy Pointer Mapping directly to tensor memory
    const float* float_data = reinterpret_cast<const float*>(
        raw_network_frame.data() + sizeof(SlabHeader)
    );

    // Bypasses user-space allocations; maps float memory directly
    target_tensor->copy_from_host(float_data, expected_payload_bytes);
    return true;
}

} // namespace sentinel::lab
```

