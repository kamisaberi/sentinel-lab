#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct SpaceAvionicsPoint
    {
        double timestamp_sec{0.0};

        // Domain 14: Aircraft & Avionics Sensors (#092, #093, #097)
        double pitot_dynamic_pressure_kpa{14.2};     // #092 (Dynamic pressure q)
        double aoa_vane_alpha1_deg{4.2};             // #093 (Primary Angle-of-Attack)
        double aoa_vane_alpha2_deg{4.4};             // #093 (Secondary Angle-of-Attack, delta <= 5.5 deg)
        double cosmic_ray_proton_flux_pfu{8.5};      // #097 (Proton storm limit <= 100 pfu)

        // Domain 29: Space Weather & Electrostatics (#199, #201)
        double solar_wind_density_p_cm3{12.4};       // #199 (Solar wind density <= 50 p/cm3)
        double spacecraft_frame_charge_v{-85.0};     // #201 (Absolute charging <= 500 V)

        // Domain 40: Spacecraft ECLSS Life Support (#278 - #280)
        double cabin_o2_partial_press_kpa{21.1};     // #278 (Safe O2: 20.0 - 22.0 kPa)
        double cabin_co2_partial_press_kpa{0.25};    // #279 (Safe CO2 <= 0.40 kPa)
        double cabin_pressure_decay_rate_kpa_m{0.01};// #280 (Depressurization limit <= 0.10 kPa/min)

        // Domain 45: Rocket Propulsion & TVC Gimbaling (#313, #318)
        double tvc_gimbal_command_deg{3.50};         // #313 (Commended gimbal angle)
        double tvc_gimbal_actual_deg{3.52};          // #313 (Tracking error <= 0.10 deg)
        double lox_tank_ullage_pressure_bar{3.2};    // #318 (Cryogenic ullage pressure >= 2.5 bar)

        // Domain 73: Grid Fins & Landing Mechanics (#513, #517)
        double grid_fin_tracking_error_deg{0.04};    // #513 (Fin error <= 0.15 deg)
        bool landing_leg_overcenter_locked{true};    // #517 (All 4 legs collet-locked)
    };

    struct SpaceAvionicsMetrics
    {
        bool valid{false};
        uint32_t total_space_sensors_audited{12};
        uint32_t passed_sensors_count{12};
        uint32_t violated_sensors_count{0};
        bool flight_envelope_or_life_breach{false};
        std::string summary_report;
    };

    struct SpaceAvionicsConfig
    {
        std::string config_path{"configs/space_avionics_profile.json"};
        double max_aoa_disparity_deg{5.5};
        double max_cosmic_ray_pfu{100.0};
        double max_frame_charge_v{500.0};
        double min_cabin_o2_kpa{20.0};
        double max_cabin_o2_kpa{22.0};
        double max_cabin_co2_kpa{0.40};
        double max_tvc_tracking_error_deg{0.10};
        double max_grid_fin_error_deg{0.15};
    };

    class SpaceAvionicsAuditor
    {
    public:
        explicit SpaceAvionicsAuditor(SpaceAvionicsConfig config = {});
        ~SpaceAvionicsAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const SpaceAvionicsPoint &sample);
        [[nodiscard]] SpaceAvionicsMetrics evaluate_space_metrics() const;
        void reset() noexcept;

    private:
        SpaceAvionicsConfig config_;
        std::vector<SpaceAvionicsPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using SpaceAvionicsAuditor = sentinel::lab::SpaceAvionicsAuditor;
    using SpaceAvionicsMetrics = sentinel::lab::SpaceAvionicsMetrics;
    using SpaceAvionicsConfig = sentinel::lab::SpaceAvionicsConfig;
    using SpaceAvionicsPoint = sentinel::lab::SpaceAvionicsPoint;
}