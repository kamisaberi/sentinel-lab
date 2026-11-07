#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct FluidShockPoint
    {
        double timestamp_sec{0.0};

        // Domain 09: Fluid, Pressure & Hydrodynamic Sensors (#052 - #057)
        double pipeline_pressure_bar{42.5};          // #052 (Normal operating: 40 - 55 bar)
        double joukowsky_dp_rate_bar_s{12.4};        // #052 (Water-hammer dP/dt limit <= 50.0 bar/s)
        double coriolis_mass_balance_ratio{0.994};   // #053 (Mass balance ratio >= 0.98)
        double subsea_apb_casing_stress_mpa{245.0};  // #054 (Safe casing hoop stress <= 450 MPa)
        double dam_cavitation_index_sigma{0.48};     // #057 (Safe cavitation index sigma >= 0.20)

        // Domain 27: Nuclear Core Two-Phase Hydrodynamics (#185, #186, #188, #189)
        double core_void_fraction_alpha{0.12};       // #185 (Max allowable alpha <= 0.35)
        double cet_subcooling_margin_c{24.8};        // #188 (Subcooling margin >= 15.0 deg C)
        double primary_loop_leak_rate_lpm{0.05};     // #189 (Technical specification limit <= 1.0 L/min)

        // Domain 37: Subsea Intervention & Hydrate Management (#256 - #258)
        double meg_injection_ratio{1.15};            // #256 (Hammerschmidt ratio >= 1.0)
        double choke_sand_erosion_kg_day{2.4};       // #257 (Max erosion rate <= 10.0 kg/day)
        double bop_accumulator_usable_vol_pct{94.2}; // #258 (Usable hydraulic reserve >= 80.0%)
    };

    struct FluidShockMetrics
    {
        bool valid{false};
        uint32_t total_fluid_sensors_audited{11};
        uint32_t passed_sensors_count{11};
        uint32_t violated_sensors_count{0};
        bool hydrodynamic_or_containment_breach{false};
        std::string summary_report;
    };

    struct FluidShockConfig
    {
        std::string config_path{"configs/fluid_shock_profile.json"};
        double max_joukowsky_dp_rate_bar_s{50.0};
        double max_casing_stress_mpa{450.0};
        double min_cavitation_index_sigma{0.20};
        double max_core_void_fraction{0.35};
        double min_subcooling_margin_c{15.0};
        double max_primary_leak_lpm{1.0};
    };

    class FluidShockAuditor
    {
    public:
        explicit FluidShockAuditor(FluidShockConfig config = {});
        ~FluidShockAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const FluidShockPoint &sample);
        [[nodiscard]] FluidShockMetrics evaluate_fluid_metrics() const;
        void reset() noexcept;

    private:
        FluidShockConfig config_;
        std::vector<FluidShockPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using FluidShockAuditor = sentinel::lab::FluidShockAuditor;
    using FluidShockMetrics = sentinel::lab::FluidShockMetrics;
    using FluidShockConfig = sentinel::lab::FluidShockConfig;
    using FluidShockPoint = sentinel::lab::FluidShockPoint;
}