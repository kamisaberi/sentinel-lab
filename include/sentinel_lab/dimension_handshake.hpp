#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <string>
#include <optional>

namespace sentinel::lab {

inline constexpr uint32_t SLAB_MAGIC = 0x534C4142; // ASCII: 'S', 'L', 'A', 'B'

#pragma pack(push, 1)
struct SlabHeader {
    uint32_t magic;          // Byte 0-3  : 0x534C4142
    uint64_t event_id;       // Byte 4-11 : Monotonic sequence identifier
    uint32_t ground_truth;   // Byte 12-15: 0 = Benign, 1 = Attack
    uint32_t dimensions;     // Byte 16-19: Feature count D
    uint32_t flags;          // Byte 20-23: Bitmask flags / Reserved
};
#pragma pack(pop)

static_assert(sizeof(SlabHeader) == 24, "SlabHeader must be exactly 24 bytes");

struct DimensionValidationResult {
    bool valid{false};
    uint32_t dimensions{0};
    size_t expected_total_bytes{0};
    std::string error_message;
};

class DimensionHandshake {
public:
    DimensionHandshake() = default;
    ~DimensionHandshake() = default;

    /// Validates raw SLAB frame header and calculates payload byte offsets dynamically
    [[nodiscard]] static DimensionValidationResult validate_frame(std::span<const uint8_t> frame_bytes) noexcept;

    /// Extract continuous float32 feature tensor span directly from raw frame with zero-copy
    [[nodiscard]] static std::span<const float> extract_features(std::span<const uint8_t> frame_bytes, uint32_t dimensions) noexcept;
};

} // namespace sentinel::lab