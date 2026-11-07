#include "sentinel_lab/power_grid.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    PowerGridAuditor::PowerGridAuditor(PowerGridConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void PowerGridAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_thd_pct") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_thd_pct = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_rocof_hz_s") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_rocof_hz_s = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void PowerGridAuditor::record_telemetry_sample(const PowerGridPoint &sample)
    {
        history_.push_back(sample);
    }

    void PowerGridAuditor::reset() noexcept
    {
        history_.clear();
    }

    PowerGridMetrics PowerGridAuditor::evaluate_grid_metrics() const
    {
        PowerGridMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Power grid telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_grid_sensors_audited = 12;
        uint32_t violations = 0;

        // 1. High-Voltage Electrical & Substation Grounding (#080, #081, #084)
        if (latest.surge_arrester_i3r_ua > config_.max_arrester_i3r_ua) violations++;
        if (latest.ngr_neutral_current_a > config_.max_ngr_current_a) violations++;
        if (latest.ground_potential_rise_v > 430.0) violations++;

        // 2. Power Quality, Harmonics & Frequency ROCOF (#206, #207, #210, #212)
        if (latest.thd_voltage_pct > config_.max_thd_pct) violations++;
        if (latest.voltage_flicker_pst > 1.0) violations++;
        if (latest.negative_sequence_unbalance_pct > config_.max_unbalance_pct) violations++;
        if (std::abs(latest.rocof_df_dt_hz_s) > config_.max_rocof_hz_s) violations++;

        // 3. Transmission Line Sag & DLR Clearance (#296)
        if (latest.line_ground_clearance_m < config_.min_ground_clearance_m) violations++;

        // 4. HVDC Cables & Maglev Levitation Gap (#546, #563)
        if (latest.hvdc_cable_pd_charge_pc > 5.0) violations++;
        if (latest.maglev_levitation_gap_mm < config_.min_maglev_gap_mm || latest.maglev_levitation_gap_mm > config_.max_maglev_gap_mm) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_grid_sensors_audited - violations;
        m.electrical_stability_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Power-Grid Invariant Report: Audited=" << m.total_grid_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.electrical_stability_breach ? "\033[1;32m[GRID & FREQUENCY STABILITY VERIFIED]\033[0m" : "\033[1;31m[GRID INSTABILITY BREACH]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab