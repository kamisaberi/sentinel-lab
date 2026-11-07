#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct PowerGridPoint
    {
        double timestamp_sec{0.0};

        // Domain 12: High-Voltage Electrical & Insulation (#080, #081, #084)
        double surge_arrester_i3r_ua{18.4};          // #080 (Leakage current <= 50.0 uA)
        double ngr_neutral_current_a{0.45};          // #081 (Neutral current <= 5.0 A)
        double ground_potential_rise_v{42.0};        // #084 (GPR limit <= 430.0 V)

        // Domain 30: Power Quality, Harmonics & ROCOF (#206, #207, #210, #212)
        double grid_frequency_hz{50.01};             // #212 (Nominal 50.0 or 60.0 Hz)
        double rocof_df_dt_hz_s{0.08};               // #212 (ROCOF limit <= 1.0 Hz/s)
        double thd_voltage_pct{1.85};                // #206 (IEEE 519 limit <= 5.0%)
        double voltage_flicker_pst{0.42};            // #207 (Flicker limit <= 1.0)
        double negative_sequence_unbalance_pct{0.65};// #210 (Unbalance U2 <= 2.0%)

        // Domain 42: Transmission Catenary & DLR (#292, #296)
        double line_ground_clearance_m{12.8};        // #296 (Minimum safe clearance >= 10.5 m)
        double catenary_tension_kn{42.5};            // #292 (Mechanical tension within design band)

        // Domain 77 & 80: HVDC Cables & Maglev Levitation (#546, #563)
        double hvdc_cable_pd_charge_pc{1.8};         // #546 (Safe partial discharge <= 5.0 pC)
        double maglev_levitation_gap_mm{10.2};       // #563 (Safe levitation: 8.0 - 12.0 mm)
    };

    struct PowerGridMetrics
    {
        bool valid{false};
        uint32_t total_grid_sensors_audited{12};
        uint32_t passed_sensors_count{12};
        uint32_t violated_sensors_count{0};
        bool electrical_stability_breach{false};
        std::string summary_report;
    };

    struct PowerGridConfig
    {
        std::string config_path{"configs/power_grid_profile.json"};
        double max_arrester_i3r_ua{50.0};
        double max_ngr_current_a{5.0};
        double max_rocof_hz_s{1.0};
        double max_thd_pct{5.0};
        double max_unbalance_pct{2.0};
        double min_ground_clearance_m{10.5};
        double min_maglev_gap_mm{8.0};
        double max_maglev_gap_mm{12.0};
    };

    class PowerGridAuditor
    {
    public:
        explicit PowerGridAuditor(PowerGridConfig config = {});
        ~PowerGridAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const PowerGridPoint &sample);
        [[nodiscard]] PowerGridMetrics evaluate_grid_metrics() const;
        void reset() noexcept;

    private:
        PowerGridConfig config_;
        std::vector<PowerGridPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using PowerGridAuditor = sentinel::lab::PowerGridAuditor;
    using PowerGridMetrics = sentinel::lab::PowerGridMetrics;
    using PowerGridConfig = sentinel::lab::PowerGridConfig;
    using PowerGridPoint = sentinel::lab::PowerGridPoint;
}