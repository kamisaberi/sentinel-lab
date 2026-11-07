#include "sentinel_lab/fluid_shock.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    FluidShockAuditor::FluidShockAuditor(FluidShockConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void FluidShockAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_joukowsky_dp_rate_bar_s") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_joukowsky_dp_rate_bar_s = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_casing_stress_mpa") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_casing_stress_mpa = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void FluidShockAuditor::record_telemetry_sample(const FluidShockPoint &sample)
    {
        history_.push_back(sample);
    }

    void FluidShockAuditor::reset() noexcept
    {
        history_.clear();
    }

    FluidShockMetrics FluidShockAuditor::evaluate_fluid_metrics() const
    {
        FluidShockMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Fluid/Hydrodynamic telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_fluid_sensors_audited = 11;
        uint32_t violations = 0;

        // 1. Pipeline & Hydrodynamic Invariants (#052, #053, #054, #057)
        if (latest.joukowsky_dp_rate_bar_s > config_.max_joukowsky_dp_rate_bar_s) violations++;
        if (latest.coriolis_mass_balance_ratio < 0.98) violations++;
        if (latest.subsea_apb_casing_stress_mpa > config_.max_casing_stress_mpa) violations++;
        if (latest.dam_cavitation_index_sigma < config_.min_cavitation_index_sigma) violations++;

        // 2. Nuclear Two-Phase Hydrodynamics (#185, #188, #189)
        if (latest.core_void_fraction_alpha > config_.max_core_void_fraction) violations++;
        if (latest.cet_subcooling_margin_c < config_.min_subcooling_margin_c) violations++;
        if (latest.primary_loop_leak_rate_lpm > config_.max_primary_leak_lpm) violations++;

        // 3. Subsea Intervention & Hydrate Management (#256 - #258)
        if (latest.meg_injection_ratio < 1.0) violations++;
        if (latest.choke_sand_erosion_kg_day > 10.0) violations++;
        if (latest.bop_accumulator_usable_vol_pct < 80.0) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_fluid_sensors_audited - violations;
        m.hydrodynamic_or_containment_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Fluid-Shock Invariant Report: Audited=" << m.total_fluid_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.hydrodynamic_or_containment_breach ? "\033[1;32m[HYDRODYNAMIC BOUNDARIES SAFE]\033[0m" : "\033[1;31m[HYDRODYNAMIC SHOCK BREACH]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab