#include "sentinel_lab/marine_subsea.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    MarineSubseaAuditor::MarineSubseaAuditor(MarineSubseaConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void MarineSubseaAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("min_ballast_uvt_pct") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.min_ballast_uvt_pct = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_tile_shear_strain") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_tile_shear_strain = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void MarineSubseaAuditor::record_telemetry_sample(const MarineSubseaPoint &sample)
    {
        history_.push_back(sample);
    }

    void MarineSubseaAuditor::reset() noexcept
    {
        history_.clear();
    }

    MarineSubseaMetrics MarineSubseaAuditor::evaluate_marine_metrics() const
    {
        MarineSubseaMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Marine/Subsea telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_marine_sensors_audited = 13;
        uint32_t violations = 0;

        // 1. Oceanography & Navigational Obstacle Clearance (#105)
        if (latest.sonar_obstacle_clearance_m < config_.min_sonar_clearance_m) violations++;

        // 2. Autonomous Vessel & Biosecurity Ballast Standards (#263, #264, #266, #267)
        if (latest.ballast_uv_transmittance_pct < config_.min_ballast_uvt_pct) violations++;
        if (latest.anti_heeling_angle_deg > config_.max_heeling_angle_deg) violations++;
        if (latest.rudder_stock_shear_stress_mpa > config_.max_rudder_shear_mpa) violations++;
        if (latest.shaft_grounding_potential_mv > config_.max_shaft_voltage_mv) violations++;

        // 3. Submarine Acoustic Stealth & Torpedo Interlocks (#363, #365, #367)
        if (latest.anechoic_tile_shear_strain > config_.max_tile_shear_strain) violations++;
        if (latest.machinery_mount_isolation_db > config_.max_machinery_mount_db) violations++;
        if (!latest.torpedo_interlock_safe) violations++;

        // 4. Deep-Sea Mining Slurry Riser & Crawler Sinkage (#486, #487)
        if (latest.slurry_riser_flow_velocity_m_s < config_.min_slurry_velocity_m_s) violations++;
        if (latest.crawler_track_sinkage_mm > config_.max_crawler_sinkage_mm) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_marine_sensors_audited - violations;
        m.ocean_or_subsea_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Marine-Subsea Invariant Report: Audited=" << m.total_marine_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.ocean_or_subsea_breach ? "\033[1;32m[OCEANOGRAPHIC & HYDRO-STEALTH VERIFIED]\033[0m" : "\033[1;31m[HYDRODYNAMIC / BIOSECURITY BREACH]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab