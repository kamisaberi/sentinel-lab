#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    struct PowerTracePoint
    {
        uint64_t timestamp_ns{0};
        double power_mw{0.0};
        double em_field_uT{0.0};
    };

    struct SideChannelMetrics
    {
        bool valid{false};
        double pearson_correlation_rho{0.0};
        double max_power_mw{0.0};
        double max_em_uT{0.0};
        bool sidechannel_anomaly_detected{false};
        std::string summary_report;
    };

    struct SideChannelConfig
    {
        std::string config_path{"configs/sidechannel_profile.json"};
        double max_power_mw_limit{450.0};
        double min_correlation_rho{0.85};
    };

    class SideChannelCorrelator
    {
    public:
        explicit SideChannelCorrelator(SideChannelConfig config = {});
        ~SideChannelCorrelator() = default;

        /// Computes Pearson Correlation Coefficient (rho) between network rate and power/EM traces
        [[nodiscard]] SideChannelMetrics evaluate_correlation(
            std::span<const double> network_packet_rates,
            std::span<const PowerTracePoint> power_traces) const;

        /// Helper to calculate Pearson correlation coefficient between two numeric vectors
        [[nodiscard]] static double calculate_pearson_rho(std::span<const double> x, std::span<const double> y) noexcept;

    private:
        SideChannelConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using SideChannelCorrelator = sentinel::lab::SideChannelCorrelator;
    using SideChannelMetrics = sentinel::lab::SideChannelMetrics;
    using SideChannelConfig = sentinel::lab::SideChannelConfig;
    using PowerTracePoint = sentinel::lab::PowerTracePoint;
}