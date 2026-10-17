#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <span>
#include <optional>

namespace sentinel::lab
{

    struct EvtResult
    {
        bool valid{false};
        double mean_latency_ns{0.0};
        double max_observed_latency_ns{0.0};
        double gumbel_location_mu{0.0};
        double gumbel_scale_sigma{0.0};
        double p99_latency_ns{0.0};
        double p99_9_latency_ns{0.0};
        double p99_999_wcet_bound_ns{0.0};
        bool deadline_satisfied{true};
        std::string summary_text;
    };

    struct EvtConfig
    {
        size_t block_size{1000};
        double confidence_level{0.99999}; // 99.999%
        double deadline_ns{100000.0};     // 100 µs
    };

    class EvtEngine
    {
    public:
        explicit EvtEngine(EvtConfig config = {});
        ~EvtEngine() = default;

        /// Feeds a high-resolution nanosecond latency observation into the sample buffer
        void record_sample(uint64_t latency_ns);

        /// Computes Extreme Value Theory (EVT) Gumbel tail parameters and WCET bounds
        [[nodiscard]] EvtResult calculate_wcet_bound() const;

        /// Reset internal sample buffer
        void reset() noexcept;

        [[nodiscard]] size_t sample_count() const noexcept;

    private:
        EvtConfig config_;
        std::vector<uint64_t> samples_;

        [[nodiscard]] std::vector<double> extract_block_maxima(std::span<const uint64_t> samples, size_t block_size) const;
    };

} // namespace sentinel::lab