#include "sentinel_lab/fab_extreme.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    FabExtremeAuditor::FabExtremeAuditor(FabExtremeConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void FabExtremeAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_tin_droplet_error_um") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_tin_droplet_error_um = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_keyhole_aspect_ratio") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_keyhole_aspect_ratio = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void FabExtremeAuditor::record_telemetry_sample(const FabExtremePoint &sample)
    {
        history_.push_back(sample);
    }

    void FabExtremeAuditor::reset() noexcept
    {
        history_.clear();
    }

    FabExtremeMetrics FabExtremeAuditor::evaluate_fab_metrics() const
    {
        FabExtremeMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Fab/Metallurgy telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_fab_sensors_audited = 11;
        uint32_t violations = 0;

        // 1. Semiconductor Cleanroom & High-Vacuum (#106, #111)
        if (latest.rga_vacuum_leak_rate_mbar_l_s > config_.max_rga_leak_rate) violations++;
        if (latest.cleanroom_cpc_nanoparticles_m3 > 35) violations++;

        // 2. Liquid Steel Continuous Casting (#166, #168)
        if (latest.mold_level_velocity_mm_s > config_.max_mold_level_speed_mm_s) violations++;
        if (latest.liquid_steel_temperature_c < 1540.0 || latest.liquid_steel_temperature_c > 1560.0) violations++;

        // 3. Metal 3D Printing Melt-Pool Dynamics (#213, #214, #217)
        if (latest.meltpool_pyrometry_temp_c < config_.min_meltpool_temp_c || latest.meltpool_pyrometry_temp_c > config_.max_meltpool_temp_c) violations++;
        if (latest.meltpool_keyhole_aspect_ratio > config_.max_keyhole_aspect_ratio) violations++;
        if (latest.build_chamber_o2_ppm > config_.max_chamber_o2_ppm) violations++;

        // 4. EUV Lithography Tin-Plasma & Collector Optics (#371, #374)
        if (latest.tin_droplet_position_error_um > config_.max_tin_droplet_error_um) violations++;
        if (latest.euv_collector_reflectivity_pct < config_.min_euv_reflectivity_pct) violations++;

        // 5. Perovskite Tandem Solar Cells (#442, #444)
        if (latest.photoluminescence_peak_nm < 745.0 || latest.photoluminescence_peak_nm > 755.0) violations++;
        if (latest.c_afm_pinhole_leakage_pa > 100.0) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_fab_sensors_audited - violations;
        m.microfabrication_defect_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Fab-Extreme Invariant Report: Audited=" << m.total_fab_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.microfabrication_defect_breach ? "\033[1;32m[NANOMETER & METALLURGY INVARIANTS PASS]\033[0m" : "\033[1;31m[NANOSCALE DEFECT / BURNOUT DETECTED]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab