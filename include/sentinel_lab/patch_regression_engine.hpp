#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    struct PatchMetricsSnapshot
    {
        double throughput_eps{0.0};
        double mean_latency_ns{0.0};
        double attack_neutralization_rate_pct{0.0}; // Drop rate on exploit stream
        size_t false_positives_count{0};
    };

    struct PatchRegressionScorecard
    {
        bool valid{false};
        bool patch_approved{false};
        std::string package_id;
        double neutralization_rate_pct{0.0}; // Target 100%
        double throughput_delta_pct{0.0};    // Pre vs Post EPS drop %
        double latency_delta_ns{0.0};        // Pre vs Post execution time delta
        size_t false_positive_delta{0};
        std::string scorecard_summary;
    };

    struct PatchRegressionConfig
    {
        std::string config_path{"configs/patch_regression_profile.json"};
        double min_neutralization_pct{100.0};
        double max_throughput_drop_pct{0.5};
        double max_latency_delta_ns{120.0};
    };

    class PatchRegressionEngine
    {
    public:
        explicit PatchRegressionEngine(PatchRegressionConfig config = {});
        ~PatchRegressionEngine() = default;

        /// Calculates differential regression scorecard comparing pre-patch and post-patch runs
        [[nodiscard]] PatchRegressionScorecard evaluate_patch(
            const std::string &package_id,
            const PatchMetricsSnapshot &pre_patch,
            const PatchMetricsSnapshot &post_patch) const;

    private:
        PatchRegressionConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using PatchRegressionEngine = sentinel::lab::PatchRegressionEngine;
    using PatchRegressionScorecard = sentinel::lab::PatchRegressionScorecard;
    using PatchMetricsSnapshot = sentinel::lab::PatchMetricsSnapshot;
    using PatchRegressionConfig = sentinel::lab::PatchRegressionConfig;
}