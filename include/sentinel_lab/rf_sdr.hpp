#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct RfTelemetryPoint
    {
        double timestamp_sec{0.0};

        // Domain 04: RF, GNSS & Electromagnetic (Sensors #023 - #028)
        double gnss_cn0_dbhz{44.5};             // #023 (Nominal: 38 - 48 dB-Hz)
        double gnss_cn0_variance{12.4};         // #023 (Spoofing has unnaturally flat variance < 1.0)
        double pps_phase_error_ns{18.5};        // #024 (Phase error <= 100 ns)
        double rf_noise_floor_elevation_db{2.1};// #025 (Jamming: noise elevation > 20 dB)
        double antenna_vswr{1.25};              // #026 (Safe VSWR <= 2.0:1)
        double near_field_magnetic_ut{0.05};    // #027 (Tempest EM leakage threshold)
        double constellation_evm_pct{3.2};      // #028 (Safe modulation EVM <= 8.5%)

        // Domain 19: High-Energy Laser / Optical (Sensors #128, #129, #131)
        double laser_pulse_energy_j{1.20};      // #128 (Normal energy threshold)
        double beam_centroid_drift_um{1.8};     // #129 (Target centroid drift <= 5.0 um)
        double optical_return_loss_db{-28.5};   // #131 (Safe reflection <= -25.0 dB)

        // Domain 65: Directed-Energy HPEM / HPM (Sensors #456, #457)
        double d_dot_electric_field_v_per_m_s{1.2e11}; // #456 (HPM surge > 1.0e14 V/(m*s))
        double rf_log_detector_power_dbm{-12.0};       // #457 (Safe front-end power)
    };

    struct RfSdrMetrics
    {
        bool valid{false};
        uint32_t total_rf_sensors_audited{12};
        uint32_t passed_sensors_count{12};
        uint32_t violated_sensors_count{0};
        bool rf_or_directed_energy_breach{false};
        std::string summary_report;
    };

    struct RfSdrConfig
    {
        std::string config_path{"configs/rf_sdr_profile.json"};
        double max_pps_phase_error_ns{100.0};
        double max_rf_noise_elevation_db{20.0};
        double max_vswr{2.0};
        double max_evm_pct{8.5};
        double max_d_dot_v_per_m_s{1.0e14};
    };

    class RfSdrAuditor
    {
    public:
        explicit RfSdrAuditor(RfSdrConfig config = {});
        ~RfSdrAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const RfTelemetryPoint &sample);
        [[nodiscard]] RfSdrMetrics evaluate_rf_metrics() const;
        void reset() noexcept;

    private:
        RfSdrConfig config_;
        std::vector<RfTelemetryPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using RfSdrAuditor = sentinel::lab::RfSdrAuditor;
    using RfSdrMetrics = sentinel::lab::RfSdrMetrics;
    using RfSdrConfig = sentinel::lab::RfSdrConfig;
    using RfTelemetryPoint = sentinel::lab::RfTelemetryPoint;
}