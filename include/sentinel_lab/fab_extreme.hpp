#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct FabExtremePoint
    {
        double timestamp_sec{0.0};

        // Domain 16: Semiconductor Cleanroom & Vacuum Sensors (#106, #111)
        double rga_vacuum_leak_rate_mbar_l_s{2.5e-12}; // #106 (Max allowable < 1.0e-11)
        uint32_t cleanroom_cpc_nanoparticles_m3{12};    // #111 (ISO 14644-1 Class 1 limit <= 35)

        // Domain 24: Continuous Casting Metallurgy (#168)
        double mold_level_velocity_mm_s{4.2};          // #168 (Breakout hazard limit <= 20.0 mm/s)
        double liquid_steel_temperature_c{1548.0};     // #166 (Target steel: 1540 - 1560 C)

        // Domain 31: Metal 3D Printing Melt-Pool Sensors (#213, #214, #217)
        double meltpool_pyrometry_temp_c{1825.0};      // #213 (Allowed melt range: 1650 - 2200 C)
        double meltpool_keyhole_aspect_ratio{1.65};    // #214 (Keyhole porosity threshold <= 2.50)
        double build_chamber_o2_ppm{42.0};             // #217 (Titanium oxidation limit <= 100 ppm)

        // Domain 53: EUV Lithography & Tin Plasma (#371, #374)
        double tin_droplet_position_error_um{0.45};    // #371 (Laser intercept precision <= 1.50 um)
        double euv_collector_reflectivity_pct{69.4};   // #374 (Degradation threshold >= 68.5%)

        // Domain 63: Perovskite Tandem Solar Cell Fab (#442, #444)
        double photoluminescence_peak_nm{751.2};       // #442 (Nominal bandgap: 745 - 755 nm)
        double c_afm_pinhole_leakage_pa{45.0};         // #444 (Max pinhole current <= 100.0 pA)
    };

    struct FabExtremeMetrics
    {
        bool valid{false};
        uint32_t total_fab_sensors_audited{11};
        uint32_t passed_sensors_count{11};
        uint32_t violated_sensors_count{0};
        bool microfabrication_defect_breach{false};
        std::string summary_report;
    };

    struct FabExtremeConfig
    {
        std::string config_path{"configs/fab_extreme_profile.json"};
        double max_rga_leak_rate{1.0e-11};
        double max_mold_level_speed_mm_s{20.0};
        double min_meltpool_temp_c{1650.0};
        double max_meltpool_temp_c{2200.0};
        double max_keyhole_aspect_ratio{2.50};
        double max_chamber_o2_ppm{100.0};
        double max_tin_droplet_error_um{1.50};
        double min_euv_reflectivity_pct{68.5};
    };

    class FabExtremeAuditor
    {
    public:
        explicit FabExtremeAuditor(FabExtremeConfig config = {});
        ~FabExtremeAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const FabExtremePoint &sample);
        [[nodiscard]] FabExtremeMetrics evaluate_fab_metrics() const;
        void reset() noexcept;

    private:
        FabExtremeConfig config_;
        std::vector<FabExtremePoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using FabExtremeAuditor = sentinel::lab::FabExtremeAuditor;
    using FabExtremeMetrics = sentinel::lab::FabExtremeMetrics;
    using FabExtremeConfig = sentinel::lab::FabExtremeConfig;
    using FabExtremePoint = sentinel::lab::FabExtremePoint;
}