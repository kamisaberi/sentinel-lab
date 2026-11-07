#include "sentinel_lab/space_avionics.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    SpaceAvionicsAuditor::SpaceAvionicsAuditor(SpaceAvionicsConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void SpaceAvionicsAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_aoa_disparity_deg") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_aoa_disparity_deg = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_tvc_tracking_error_deg") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_tvc_tracking_error_deg = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void SpaceAvionicsAuditor::record_telemetry_sample(const SpaceAvionicsPoint &sample)
    {
        history_.push_back(sample);
    }

    void SpaceAvionicsAuditor::reset() noexcept
    {
        history_.clear();
    }

    SpaceAvionicsMetrics SpaceAvionicsAuditor::evaluate_space_metrics() const
    {
        SpaceAvionicsMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Space/Avionics telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_space_sensors_audited = 12;
        uint32_t violations = 0;

        // 1. Aircraft Avionics & AoA Vane Agreement (#092, #093, #097)
        double aoa_delta = std::abs(latest.aoa_vane_alpha1_deg - latest.aoa_vane_alpha2_deg);
        if (aoa_delta > config_.max_aoa_disparity_deg) violations++;
        if (latest.cosmic_ray_proton_flux_pfu > config_.max_cosmic_ray_pfu) violations++;

        // 2. Space Weather & Electrostatic Frame Potential (#199, #201)
        if (latest.solar_wind_density_p_cm3 > 50.0) violations++;
        if (std::abs(latest.spacecraft_frame_charge_v) > config_.max_frame_charge_v) violations++;

        // 3. Cabin Life Support (ECLSS) Partial Pressures (#278 - #280)
        if (latest.cabin_o2_partial_press_kpa < config_.min_cabin_o2_kpa || latest.cabin_o2_partial_press_kpa > config_.max_cabin_o2_kpa) violations++;
        if (latest.cabin_co2_partial_press_kpa > config_.max_cabin_co2_kpa) violations++;
        if (latest.cabin_pressure_decay_rate_kpa_m > 0.10) violations++;

        // 4. Rocket TVC Gimbal & Propellant Tank Integrity (#313, #318)
        double tvc_error = std::abs(latest.tvc_gimbal_command_deg - latest.tvc_gimbal_actual_deg);
        if (tvc_error > config_.max_tvc_tracking_error_deg) violations++;
        if (latest.lox_tank_ullage_pressure_bar < 2.5) violations++;

        // 5. Grid Fins & Landing Collet Lock (#513, #517)
        if (latest.grid_fin_tracking_error_deg > config_.max_grid_fin_error_deg) violations++;
        if (!latest.landing_leg_overcenter_locked) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_space_sensors_audited - violations;
        m.flight_envelope_or_life_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Space-Avionics Invariant Report: Audited=" << m.total_space_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.flight_envelope_or_life_breach ? "\033[1;32m[FLIGHT & ORBITAL ENVELOPE VERIFIED]\033[0m" : "\033[1;31m[CRITICAL AEROSPACE BREACH]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab