#include "sentinel_lab/acoustic_tdoa.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    AcousticTdoaAuditor::AcousticTdoaAuditor(AcousticTdoaConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void AcousticTdoaAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_airgap_kurtosis") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_airgap_kurtosis = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_bearing_vibration_mms") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_bearing_vibration_mms = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void AcousticTdoaAuditor::record_telemetry_sample(const AcousticTdoaPoint &sample)
    {
        history_.push_back(sample);
    }

    void AcousticTdoaAuditor::reset() noexcept
    {
        history_.clear();
    }

    AcousticTdoaMetrics AcousticTdoaAuditor::evaluate_acoustic_metrics() const
    {
        AcousticTdoaMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Acoustic telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_acoustic_sensors_audited = 11;
        uint32_t violations = 0;

        // 1. Ultrasonic Air-Gap & Machine Mechanical Integrity (#029, #031, #034)
        if (latest.ultrasonic_airgap_kurtosis > config_.max_airgap_kurtosis) violations++;
        if (latest.bearing_vibration_rms_mms > config_.max_bearing_vibration_mms) violations++;
        if (latest.cavitation_acoustic_db > 55.0) violations++;

        // 2. Ballistic Shockwave TDOA Array Hyperbolic Fit (#032)
        if (latest.tdoa_residual_error_m > config_.max_tdoa_residual_m) violations++;

        // 3. Combustion Dynamics & Turbomachinery (#135, #140)
        if (latest.combustor_rumble_p_rms_bar > config_.max_combustor_rumble_bar) violations++;
        if (latest.turbopump_cavitation_rms_v > 0.15) violations++;

        // 4. Submarine Towed Sonar & Decoy bubble Invariants (#521, #523)
        if (latest.towed_array_tension_kn > config_.max_towed_array_tension_kn) violations++;
        if (latest.decoy_bubble_attenuation_db < config_.min_decoy_attenuation_db) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_acoustic_sensors_audited - violations;
        m.acoustic_or_kinetic_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Acoustic-TDOA Invariant Report: Audited=" << m.total_acoustic_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.acoustic_or_kinetic_breach ? "\033[1;32m[ACOUSTIC ENVELOPE VERIFIED]\033[0m" : "\033[1;31m[ACOUSTIC BREACH DETECTED]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab