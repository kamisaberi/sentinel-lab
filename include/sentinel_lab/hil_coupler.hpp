#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct PhysicalStatePoint
    {
        double timestamp_sec{0.0};
        double temperature_c{0.0};
        double vibration_rpm{0.0};
        double pressure_psi{0.0};
        bool in_safe_region{true};
    };

    struct HilMetrics
    {
        bool valid{false};
        double critical_clearing_time_ms{0.0}; // T_crit
        double max_observed_temp_c{0.0};
        double lyapunov_stability_exponent{0.0};
        bool safety_envelope_violated{false};
        std::string summary_report;
    };

    struct HilConfig
    {
        std::string config_path{"configs/hil_profile.json"};
        double max_safe_temp_c{120.0};
        double target_tcrit_sla_ms{50.0};
    };

    class HilCoupler
    {
    public:
        explicit HilCoupler(HilConfig config = {});
        ~HilCoupler() = default;

        /// Load configuration from JSON
        void load_config(const std::string &config_path);

        /// Record state point x(t) sampled from sentinel-matrix PLC twin
        void record_state_sample(const PhysicalStatePoint &point);

        /// Compute Critical Clearing Time (T_crit) and Lyapunov physical stability exponent
        [[nodiscard]] HilMetrics compute_hil_metrics(double mitigation_delay_ms) const;

        void reset() noexcept;

    private:
        HilConfig config_;
        std::vector<PhysicalStatePoint> trajectory_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using HilCoupler = sentinel::lab::HilCoupler;
    using HilMetrics = sentinel::lab::HilMetrics;
    using HilConfig = sentinel::lab::HilConfig;
    using PhysicalStatePoint = sentinel::lab::PhysicalStatePoint;
}