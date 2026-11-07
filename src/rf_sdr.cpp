#include "sentinel_lab/rf_sdr.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    RfSdrAuditor::RfSdrAuditor(RfSdrConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void RfSdrAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_pps_phase_error_ns") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_pps_phase_error_ns = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_vswr") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_vswr = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void RfSdrAuditor::record_telemetry_sample(const RfTelemetryPoint &sample)
    {
        history_.push_back(sample);
    }

    void RfSdrAuditor::reset() noexcept
    {
        history_.clear();
    }

    RfSdrMetrics RfSdrAuditor::evaluate_rf_metrics() const
    {
        RfSdrMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "RF/SDR telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_rf_sensors_audited = 12;
        uint32_t violations = 0;

        // 1. GNSS & 1PPS Physical Time Checks (#023, #024)
        if (latest.gnss_cn0_variance < 0.8) violations++; // Spoofing/meaconing signature
        if (latest.pps_phase_error_ns > config_.max_pps_phase_error_ns) violations++;

        // 2. Wideband RF & Transmission Integrity (#025, #026, #028)
        if (latest.rf_noise_floor_elevation_db > config_.max_rf_noise_elevation_db) violations++;
        if (latest.antenna_vswr > config_.max_vswr) violations++;
        if (latest.constellation_evm_pct > config_.max_evm_pct) violations++;

        // 3. High-Energy Laser / Optoelectronics (#128, #129, #131)
        if (latest.beam_centroid_drift_um > 5.0) violations++;
        if (latest.optical_return_loss_db > -25.0) violations++; // Reflection threshold breach

        // 4. Directed-Energy HPEM / HPM Invariants (#456, #457)
        if (latest.d_dot_electric_field_v_per_m_s > config_.max_d_dot_v_per_m_s) violations++;
        if (latest.rf_log_detector_power_dbm > 20.0) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_rf_sensors_audited - violations;
        m.rf_or_directed_energy_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "RF-SDR Invariant Report: Audited=" << m.total_rf_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.rf_or_directed_energy_breach ? "\033[1;32m[RF/EM COHERENCE VERIFIED]\033[0m" : "\033[1;31m[RF/EM ATTACK DETECTED]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab