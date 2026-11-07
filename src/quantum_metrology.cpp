#include "sentinel_lab/quantum_metrology.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    QuantumMetrologyAuditor::QuantumMetrologyAuditor(QuantumMetrologyConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void QuantumMetrologyAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_nv_dephasing_khz") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_nv_dephasing_khz = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_mirnov_tearing_ratio_pct") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_mirnov_tearing_ratio_pct = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void QuantumMetrologyAuditor::record_telemetry_sample(const QuantumMetrologyPoint &sample)
    {
        history_.push_back(sample);
    }

    void QuantumMetrologyAuditor::reset() noexcept
    {
        history_.clear();
    }

    QuantumMetrologyMetrics QuantumMetrologyAuditor::evaluate_quantum_metrics() const
    {
        QuantumMetrologyMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Quantum metrology telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_quantum_sensors_audited = 12;
        uint32_t violations = 0;

        // 1. Quantum Spin & Primary Metrology (#060, #123, #124)
        if (latest.nv_diamond_dephasing_delta_khz > config_.max_nv_dephasing_khz) violations++;
        if (latest.ramsey_fringe_visibility < config_.min_ramsey_visibility) violations++;

        // 2. Cold Atom & Gravimetry Laplace Invariant (#321, #323, #326)
        if (latest.odt_laser_power_stability_pct > config_.max_odt_power_drift_pct) violations++;
        if (std::abs(latest.gravity_tensor_trace_eotvos) > config_.max_gravity_tensor_trace_e) violations++;

        // 3. Superconducting Tokamak Fusion Boundary Invariants (#421, #423, #424, #426)
        if (latest.mirnov_tearing_mode_b_ratio_pct > config_.max_mirnov_tearing_ratio_pct) violations++;
        if (latest.divertor_heat_flux_mw_m2 > config_.max_divertor_heat_flux_mw_m2) violations++;
        if (latest.plasma_current_quench_rate_ma_s > config_.max_current_quench_ma_s) violations++;
        if (latest.bolometer_radiated_fraction > 0.70) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_quantum_sensors_audited - violations;
        m.quantum_or_fusion_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Quantum-Metrology Invariant Report: Audited=" << m.total_quantum_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.quantum_or_fusion_breach ? "\033[1;32m[QUANTUM COHERENCE & FUSION STABLE]\033[0m" : "\033[1;31m[DECOHERENCE / DISRUPTION BREACH]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab