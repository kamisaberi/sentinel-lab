#include "sentinel_lab/chemical_gas.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    ChemicalGasAuditor::ChemicalGasAuditor(ChemicalGasConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void ChemicalGasAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("min_water_ph") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.min_water_ph = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_water_ph") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_water_ph = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void ChemicalGasAuditor::record_telemetry_sample(const ChemicalGasPoint &sample)
    {
        history_.push_back(sample);
    }

    void ChemicalGasAuditor::reset() noexcept
    {
        history_.clear();
    }

    ChemicalGasMetrics ChemicalGasAuditor::evaluate_chemical_metrics() const
    {
        ChemicalGasMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Chemical/Gas telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_chemical_sensors_audited = 13;
        uint32_t violations = 0;

        // 1. Water Treatment & Chemical Dosing Invariants (#061, #062, #162)
        if (latest.water_ph_level < config_.min_water_ph || latest.water_ph_level > config_.max_water_ph) violations++;
        if (latest.free_chlorine_residual_mg_l < config_.min_chlorine_mg_l || latest.free_chlorine_residual_mg_l > config_.max_chlorine_mg_l) violations++;
        if (latest.chlorine_dioxide_mg_l > 0.80) violations++;

        // 2. Dielectric Gas & Hydrogen Safety (#063, #064, #556, #557)
        if (latest.sf6_gas_density_kg_m3 < config_.min_sf6_density_kg_m3) violations++;
        if (latest.ambient_h2_concentration_pct > config_.max_ambient_h2_pct) violations++;
        if (latest.h2_in_o2_crossover_pct > config_.max_h2_in_o2_pct) violations++;
        if (latest.o2_in_h2_trace_ppm > 5.0) violations++;

        // 3. CBRN Chemical Warfare & Toxic Gases (#058, #157, #229, #233)
        if (latest.ammonia_nh3_ppm > 25.0) violations++;
        if (latest.saw_nerve_agent_phase_shift_deg > config_.max_nerve_agent_phase_deg) violations++;
        if (latest.h2s_sour_gas_ppm > config_.max_h2s_ppm) violations++;
        if (latest.hf_acid_path_ppm_m > 2.00) violations++;

        // 4. Alkaline Electrolyzer Lye Purity (#560)
        if (latest.koh_electrolyte_conc_pct < 29.0 || latest.koh_electrolyte_conc_pct > 31.0) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_chemical_sensors_audited - violations;
        m.chemical_or_toxicity_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Chemical-Gas Invariant Report: Audited=" << m.total_chemical_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.chemical_or_toxicity_breach ? "\033[1;32m[STOICHIOMETRIC SAFETY VERIFIED]\033[0m" : "\033[1;31m[TOXIC / FLAMMABLE HAZARD DETECTED]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab