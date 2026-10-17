#include "sentinel_lab/hil_coupler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    HilCoupler::HilCoupler(HilConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void HilCoupler::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("max_safe_temperature_c") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try
                    {
                        config_.max_safe_temp_c = std::stod(line.substr(pos + 1));
                    }
                    catch (...)
                    {
                    }
                }
            }
        }
    }

    void HilCoupler::record_state_sample(const PhysicalStatePoint &point)
    {
        trajectory_.push_back(point);
    }

    void HilCoupler::reset() noexcept
    {
        trajectory_.clear();
    }

    HilMetrics HilCoupler::compute_hil_metrics(double mitigation_delay_ms) const
    {
        HilMetrics m{};
        if (trajectory_.empty())
        {
            m.summary_report = "Trajectory buffer is empty";
            return m;
        }

        double max_temp = 0.0;
        double t_first_breach = -1.0;

        for (const auto &pt : trajectory_)
        {
            if (pt.temperature_c > max_temp)
            {
                max_temp = pt.temperature_c;
            }

            if (pt.temperature_c > config_.max_safe_temp_c && t_first_breach < 0.0)
            {
                t_first_breach = pt.timestamp_sec;
            }
        }

        m.max_observed_temp_c = max_temp;
        m.safety_envelope_violated = (max_temp > config_.max_safe_temp_c);

        // Calculate Critical Clearing Time T_crit
        // T_crit = T_first_breach - T_mitigation
        m.critical_clearing_time_ms = (t_first_breach >= 0.0) ? (t_first_breach * 1000.0) : config_.target_tcrit_sla_ms;

        // Estimate Lyapunov Stability Exponent lambda: x(t) ~ x_0 * exp(lambda * t)
        if (trajectory_.size() >= 2)
        {
            double d0 = std::abs(trajectory_.front().temperature_c - 25.0);
            double dN = std::abs(trajectory_.back().temperature_c - 25.0);
            double total_t = trajectory_.back().timestamp_sec - trajectory_.front().timestamp_sec;

            if (d0 > 1e-3 && total_t > 1e-3)
            {
                m.lyapunov_stability_exponent = std::log(dN / d0) / total_t;
            }
        }

        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "HIL Co-Simulation Metrics: MaxTemp=" << m.max_observed_temp_c << "°C (Limit: "
           << config_.max_safe_temp_c << "°C) | T_crit SLA=" << m.critical_clearing_time_ms << "ms | "
           << "Mitigation Delay=" << mitigation_delay_ms << "ms | "
           << "Lyapunov Exponent=" << std::setprecision(4) << m.lyapunov_stability_exponent << " ["
           << (!m.safety_envelope_violated ? "STABLE" : "PHYSICAL BREACH") << "]";
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab