#include "sentinel_lab/sidechannel_correlator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <numeric>
#include <algorithm>

namespace sentinel::lab
{

    SideChannelCorrelator::SideChannelCorrelator(SideChannelConfig config)
        : config_(std::move(config)) {}

    double SideChannelCorrelator::calculate_pearson_rho(std::span<const double> x, std::span<const double> y) noexcept
    {
        if (x.empty() || x.size() != y.size())
            return 0.0;

        size_t n = x.size();
        double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
        double sum_y = std::accumulate(y.begin(), y.end(), 0.0);

        double mean_x = sum_x / static_cast<double>(n);
        double mean_y = sum_y / static_cast<double>(n);

        double num = 0.0;
        double den_x = 0.0;
        double den_y = 0.0;

        for (size_t i = 0; i < n; ++i)
        {
            double dx = x[i] - mean_x;
            double dy = y[i] - mean_y;
            num += dx * dy;
            den_x += dx * dx;
            den_y += dy * dy;
        }

        if (den_x <= 0.0 || den_y <= 0.0)
            return 0.0;
        return num / (std::sqrt(den_x) * std::sqrt(den_y));
    }

    SideChannelMetrics SideChannelCorrelator::evaluate_correlation(
        std::span<const double> network_packet_rates,
        std::span<const PowerTracePoint> power_traces) const
    {
        SideChannelMetrics m{};
        if (network_packet_rates.empty() || network_packet_rates.size() != power_traces.size())
        {
            m.summary_report = "Invalid or mismatched network/power trace sizes";
            return m;
        }

        size_t n = network_packet_rates.size();
        std::vector<double> power_values(n);
        double max_p = 0.0;
        double max_em = 0.0;

        for (size_t i = 0; i < n; ++i)
        {
            power_values[i] = power_traces[i].power_mw;
            max_p = std::max(max_p, power_traces[i].power_mw);
            max_em = std::max(max_em, power_traces[i].em_field_uT);
        }

        m.max_power_mw = max_p;
        m.max_em_uT = max_em;
        m.pearson_correlation_rho = calculate_pearson_rho(network_packet_rates, power_values);

        // Flag anomaly if network is idle/low-rate while power spikes OR correlation drops below SLA threshold
        bool power_spike_on_idle = (max_p > config_.max_power_mw_limit);
        bool low_correlation = (m.pearson_correlation_rho < config_.min_correlation_rho);

        m.sidechannel_anomaly_detected = (power_spike_on_idle || low_correlation);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Hardware Side-Channel Correlation: Pearson Rho=" << m.pearson_correlation_rho
           << " (Limit: >=" << config_.min_correlation_rho << ") | Max Power=" << m.max_power_mw
           << "mW (Limit: " << config_.max_power_mw_limit << "mW) | Max EM=" << m.max_em_uT << "uT ["
           << (!m.sidechannel_anomaly_detected ? "\033[1;32m[PASSED] Physical-Cyber Correlated\033[0m" : "\033[1;31m[ANOMALY] Out-of-Band Power Spike / Decoupling\033[0m") << "]";
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab