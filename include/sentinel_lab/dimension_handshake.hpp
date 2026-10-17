#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <string>
#include <optional>
#include "sentinel_lab/slab_protocol.hpp"

namespace sentinel::lab
{

    struct DimensionValidationResult
    {
        bool valid{false};
        uint32_t dimensions{0};
        size_t expected_total_bytes{0};
        std::string error_message;
    };

    class DimensionHandshake
    {
    public:
        DimensionHandshake() = default;
        ~DimensionHandshake() = default;

        /// Validates raw SLAB frame header and calculates payload byte offsets dynamically
        [[nodiscard]] static DimensionValidationResult validate_frame(std::span<const uint8_t> frame_bytes) noexcept;

        /// Extract continuous float32 feature tensor span directly from raw frame with zero-copy
        [[nodiscard]] static std::span<const float> extract_features(std::span<const uint8_t> frame_bytes, uint32_t dimensions) noexcept;
    };

} // namespace sentinel::lab