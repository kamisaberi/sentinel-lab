#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    struct FdiaTelemetryFrame
    {
        uint64_t timestamp_ns{0};
        uint32_t cyber_sensor_id{100};
        float cyber_reported_score{0.10f}; // Spoofed NORMAL (0.10)
        uint32_t physical_sensor_id{101};
        double physical_true_value{85.0}; // True physical temperature (85°C -> 140°C)
        bool is_fdia_active{false};
    };

    struct FdiaMetrics
    {
        bool valid{false};
        double time_to_detection_ms{0.0}; // T_det
        double max_physical_drift{0.0};
        bool causal_alert_triggered{false};
        double f1_score{1.0};
        std::string summary_report;
    };

    struct FdiaConfig
    {
        std::string config_path{"configs/fdia_profile.json"};
        uint32_t cyber_sensor_id{100};
        uint32_t physical_sensor_id{101};
        double critical_threshold{120.0};
        double target_tdet_sla_ms{10.0};
    };

    class FdiaEngine
    {
    public:
        explicit FdiaEngine(FdiaConfig config = {});
        ~FdiaEngine() = default;

        /// Generates dual-stream synchronized FDIA telemetry frame
        [[nodiscard]] FdiaTelemetryFrame generate_fdia_frame(uint64_t timestamp_ns, double current_physical_temp, bool inject_blinding) const;

        /// Evaluates Causal Divergence Detection (T_det) and F1-Score
        [[nodiscard]] FdiaMetrics evaluate_causal_detection(
            std::span<const FdiaTelemetryFrame> frames,
            uint64_t detection_timestamp_ns) const;

        void reset() noexcept;

    private:
        FdiaConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using FdiaEngine = sentinel::lab::FdiaEngine;
    using FdiaMetrics = sentinel::lab::FdiaMetrics;
    using FdiaConfig = sentinel::lab::FdiaConfig;
    using FdiaTelemetryFrame = sentinel::lab::FdiaTelemetryFrame;
}