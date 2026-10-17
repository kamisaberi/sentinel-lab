#include "sentinel_lab/evt_engine.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace sentinel::lab {

EvtEngine::EvtEngine(EvtConfig config)
    : config_(config) {
    samples_.reserve(100000);
}

void EvtEngine::record_sample(uint64_t latency_ns) {
    samples_.push_back(latency_ns);
}

void EvtEngine::reset() noexcept {
    samples_.clear();
}

size_t EvtEngine::sample_count() const noexcept {
    return samples_.size();
}

std::vector<double> EvtEngine::extract_block_maxima(std::span<const uint64_t> samples, size_t block_size) const {
    std::vector<double> maxima;
    if (samples.empty() || block_size == 0) return maxima;

    size_t num_blocks = samples.size() / block_size;
    maxima.reserve(num_blocks);

    for (size_t i = 0; i < num_blocks; ++i) {
        auto start = samples.begin() + (i * block_size);
        auto end = start + block_size;
        uint64_t max_val = *std::max_element(start, end);
        maxima.push_back(static_cast<double>(max_val));
    }
    return maxima;
}

EvtResult EvtEngine::calculate_wcet_bound() const {
    EvtResult res{};
    if (samples_.size() < config_.block_size) {
        res.summary_text = "Insufficient samples for EVT estimation";
        return res;
    }

    // 1. Calculate basic statistics
    double sum = std::accumulate(samples_.begin(), samples_.end(), 0.0);
    res.mean_latency_ns = sum / static_cast<double>(samples_.size());
    res.max_observed_latency_ns = static_cast<double>(*std::max_element(samples_.begin(), samples_.end()));

    // 2. Extract Block Maxima
    std::vector<double> maxima = extract_block_maxima(samples_, config_.block_size);
    if (maxima.size() < 2) {
        res.summary_text = "Insufficient block maxima for Gumbel parameter fitting";
        return res;
    }

    // 3. Estimate Gumbel Distribution Parameters (mu, sigma) via Method of Moments
    double max_sum = std::accumulate(maxima.begin(), maxima.end(), 0.0);
    double max_mean = max_sum / static_cast<double>(maxima.size());

    double var_sum = 0.0;
    for (double val : maxima) {
        var_sum += (val - max_mean) * (val - max_mean);
    }
    double max_variance = var_sum / static_cast<double>(maxima.size() - 1);
    double std_dev = std::sqrt(max_variance);

    // Gumbel Parameter Equations: sigma = std_dev * sqrt(6) / pi, mu = max_mean - gamma * sigma
    constexpr double Euler_Mascheroni = 0.57721566490153286060;
    constexpr double Pi = 3.14159265358979323846;

    res.gumbel_scale_sigma = (std_dev * std::sqrt(6.0)) / Pi;
    res.gumbel_location_mu = max_mean - (Euler_Mascheroni * res.gumbel_scale_sigma);

    // 4. Calculate EVT Quantile Bounds
    // Cumulative Hazard Function: x_p = mu - sigma * ln(-ln(p))
    auto calculate_quantile = [this, &res](double p) {
        return res.gumbel_location_mu - (res.gumbel_scale_sigma * std::log(-std::log(p)));
    };

    res.p99_latency_ns = calculate_quantile(0.99);
    res.p99_9_latency_ns = calculate_quantile(0.999);
    res.p99_999_wcet_bound_ns = calculate_quantile(config_.confidence_level);

    res.deadline_satisfied = (res.p99_999_wcet_bound_ns <= config_.deadline_ns);
    res.valid = true;

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2)
       << "EVT WCET Analysis: Mean=" << res.mean_latency_ns << "ns | MaxObserved="
       << res.max_observed_latency_ns << "ns | Gumbel(mu=" << res.gumbel_location_mu
       << ", sigma=" << res.gumbel_scale_sigma << ") | p99.999% WCET="
       << res.p99_999_wcet_bound_ns << "ns ["
       << (res.deadline_satisfied ? "DEADLINE SATISFIED" : "DEADLINE BREACH") << "]";
    res.summary_text = ss.str();

    return res;
}

} // namespace sentinel::lab