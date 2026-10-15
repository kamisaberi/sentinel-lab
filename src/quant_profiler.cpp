#include "sentinel_lab/quant_profiler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace sentinel::lab {

QuantProfiler::QuantProfiler(QuantConfig config)
    : config_(std::move(config)) {}

double QuantProfiler::calculate_cosine_similarity(std::span<const float> fp32_logits, std::span<const float> quant_logits) noexcept {
    if (fp32_logits.empty() || fp32_logits.size() != quant_logits.size()) return 0.0;

    double dot_product = 0.0;
    double norm_a = 0.0;
    double norm_b = 0.0;

    for (size_t i = 0; i < fp32_logits.size(); ++i) {
        dot_product += static_cast<double>(fp32_logits[i]) * static_cast<double>(quant_logits[i]);
        norm_a += static_cast<double>(fp32_logits[i]) * static_cast<double>(fp32_logits[i]);
        norm_b += static_cast<double>(quant_logits[i]) * static_cast<double>(quant_logits[i]);
    }

    if (norm_a <= 0.0 || norm_b <= 0.0) return 0.0;
    return dot_product / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

double QuantProfiler::calculate_mse(std::span<const float> fp32_logits, std::span<const float> quant_logits) noexcept {
    if (fp32_logits.empty() || fp32_logits.size() != quant_logits.size()) return 0.0;

    double sum_sq_err = 0.0;
    for (size_t i = 0; i < fp32_logits.size(); ++i) {
        double diff = static_cast<double>(fp32_logits[i]) - static_cast<double>(quant_logits[i]);
        sum_sq_err += diff * diff;
    }
    return sum_sq_err / static_cast<double>(fp32_logits.size());
}

double QuantProfiler::calculate_rank_inversion_pct(std::span<const float> fp32_attr, std::span<const float> quant_attr) noexcept {
    if (fp32_attr.empty() || fp32_attr.size() != quant_attr.size()) return 0.0;

    size_t n = fp32_attr.size();
    std::vector<size_t> rank_a(n), rank_b(n);
    std::iota(rank_a.begin(), rank_a.end(), 0);
    std::iota(rank_b.begin(), rank_b.end(), 0);

    std::sort(rank_a.begin(), rank_a.end(), [&](size_t i, size_t j) { return fp32_attr[i] > fp32_attr[j]; });
    std::sort(rank_b.begin(), rank_b.end(), [&](size_t i, size_t j) { return quant_attr[i] > quant_attr[j]; });

    size_t inversions = 0;
    for (size_t i = 0; i < n; ++i) {
        if (rank_a[i] != rank_b[i]) {
            inversions++;
        }
    }

    return (static_cast<double>(inversions) / static_cast<double>(n)) * 100.0;
}

QuantDriftMetrics QuantProfiler::evaluate_backend_drift(
    const std::string& backend_name,
    const std::string& precision_type,
    std::span<const float> fp32_logits,
    std::span<const float> quant_logits,
    std::span<const float> fp32_xai_attr,
    std::span<const float> quant_xai_attr,
    double latency_us
) const {
    QuantDriftMetrics m{};
    m.backend_name = backend_name;
    m.precision_type = precision_type;
    m.avg_latency_us = latency_us;

    m.cosine_similarity = calculate_cosine_similarity(fp32_logits, quant_logits);
    m.mean_squared_error = calculate_mse(fp32_logits, quant_logits);
    m.rank_inversion_pct = calculate_rank_inversion_pct(fp32_xai_attr, quant_xai_attr);

    m.precision_sla_satisfied = (m.cosine_similarity >= config_.min_cosine_similarity) &&
                                (m.rank_inversion_pct <= config_.max_rank_inversion_pct);
    m.valid = true;

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4)
       << "Multi-Silicon Drift [" << backend_name << " (" << precision_type << ")]: "
       << "Cosine Similarity=" << m.cosine_similarity << " (Limit: " << config_.min_cosine_similarity << ") | "
       << "MSE=" << m.mean_squared_error << " | "
       << "MRD XAI Rank Inversion=" << std::setprecision(2) << m.rank_inversion_pct << "% | "
       << "Latency=" << latency_us << "us ["
       << (m.precision_sla_satisfied ? "\033[1;32m[PASSED] Quant Precision Retained\033[0m" : "\033[1;31m[FAILED] Quant Drift Breach\033[0m") << "]";
    m.summary_report = ss.str();

    return m;
}

} // namespace sentinel::lab