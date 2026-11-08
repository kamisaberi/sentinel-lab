#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct MarineSubseaPoint
    {
        double timestamp_sec{0.0};

        // Domain 15: Oceanography & Subsea Nav (#099, #100, #101, #105)
        double ocean_current_velocity_knots{2.4};   // #099 (ADCP water speed)
        double ctd_sound_speed_m_s{1512.5};         // #100 (UNESCO 1983 sound velocity)
        double subsea_depth_m{850.0};               // #101 (Resonant quartz depth)
        double sonar_obstacle_clearance_m{42.5};    // #105 (Minimum clearance barrier >= 25.0 m)

        // Domain 38: Autonomous Vessel & Ballast Biosecurity (#263, #264, #266, #267)
        double ballast_uv_transmittance_pct{78.5};  // #263 (IMO D-2 standard >= 70.0%)
        double anti_heeling_angle_deg{0.45};        // #264 (Maximum allowable heel <= 1.50 deg)
        double rudder_stock_shear_stress_mpa{142.0};// #266 (Yield limit <= 280.0 MPa)
        double shaft_grounding_potential_mv{18.5};  // #267 (Bearing erosion limit <= 50.0 mV)

        // Domain 52: Submarine Acoustic Stealth & Hull (#363, #365, #367)
        double anechoic_tile_shear_strain{0.015};   // #363 (Tile debonding threshold <= 0.050)
        double machinery_mount_isolation_db{-45.2}; // #365 (Required isolation <= -40.0 dB)
        bool torpedo_interlock_safe{true};          // #367 (Muzzle open ONLY IF breech locked)

        // Domain 69: Deep-Sea Mining Crawler & Slurry Riser (#486, #487)
        double slurry_riser_flow_velocity_m_s{3.8}; // #486 (Durand deposition velocity >= 2.8 m/s)
        double crawler_track_sinkage_mm{48.0};      // #487 (Geotechnical sinkage <= 150.0 mm)
    };

    struct MarineSubseaMetrics
    {
        bool valid{false};
        uint32_t total_marine_sensors_audited{13};
        uint32_t passed_sensors_count{13};
        uint32_t violated_sensors_count{0};
        bool ocean_or_subsea_breach{false};
        std::string summary_report;
    };

    struct MarineSubseaConfig
    {
        std::string config_path{"configs/marine_subsea_profile.json"};
        double min_sonar_clearance_m{25.0};
        double min_ballast_uvt_pct{70.0};
        double max_heeling_angle_deg{1.50};
        double max_rudder_shear_mpa{280.0};
        double max_shaft_voltage_mv{50.0};
        double max_tile_shear_strain{0.050};
        double max_machinery_mount_db{-40.0};
        double min_slurry_velocity_m_s{2.80};
        double max_crawler_sinkage_mm{150.0};
    };

    class MarineSubseaAuditor
    {
    public:
        explicit MarineSubseaAuditor(MarineSubseaConfig config = {});
        ~MarineSubseaAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const MarineSubseaPoint &sample);
        [[nodiscard]] MarineSubseaMetrics evaluate_marine_metrics() const;
        void reset() noexcept;

    private:
        MarineSubseaConfig config_;
        std::vector<MarineSubseaPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using MarineSubseaAuditor = sentinel::lab::MarineSubseaAuditor;
    using MarineSubseaMetrics = sentinel::lab::MarineSubseaMetrics;
    using MarineSubseaConfig = sentinel::lab::MarineSubseaConfig;
    using MarineSubseaPoint = sentinel::lab::MarineSubseaPoint;
}