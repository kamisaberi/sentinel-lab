#include "sentinel_lab/dimension_handshake.hpp"
#include <cstring>

namespace sentinel::lab
{

    DimensionValidationResult DimensionHandshake::validate_frame(std::span<const uint8_t> frame_bytes) noexcept
    {
        DimensionValidationResult res;

        // 1. Verify frame satisfies minimum 24-byte SlabHeader length
        if (frame_bytes.size() < sizeof(SlabHeader))
        {
            res.error_message = "Frame smaller than 24-byte SLAB header";
            return res;
        }

        const auto *header = reinterpret_cast<const SlabHeader *>(frame_bytes.data());

        // 2. Verify 0x534C4142 Magic Constant
        if (header->magic != SLAB_MAGIC)
        {
            res.error_message = "Invalid SLAB magic header constant";
            return res;
        }

        uint32_t D = header->dimensions;

        // 3. Dynamic Dimension Boundary Check (Accepts any D in 1..1024)
        if (D == 0 || D > 1024)
        {
            res.error_message = "Dimension count out of bounds [1..1024]";
            return res;
        }

        // 4. Calculate total expected packet bytes (24-byte Header + D * 4-byte Float32)
        size_t required_bytes = sizeof(SlabHeader) + (static_cast<size_t>(D) * sizeof(float));
        if (frame_bytes.size() < required_bytes)
        {
            res.error_message = "Truncated SLAB payload: insufficient bytes for dimensions";
            return res;
        }

        res.valid = true;
        res.dimensions = D;
        res.expected_total_bytes = required_bytes;
        return res;
    }

    std::span<const float> DimensionHandshake::extract_features(std::span<const uint8_t> frame_bytes, uint32_t dimensions) noexcept
    {
        if (frame_bytes.size() < sizeof(SlabHeader) + (dimensions * sizeof(float)))
        {
            return {};
        }

        const float *float_ptr = reinterpret_cast<const float *>(frame_bytes.data() + sizeof(SlabHeader));
        return {float_ptr, dimensions};
    }

} // namespace sentinel::lab