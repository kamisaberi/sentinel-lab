#include "sentinel_lab/patch_regression_engine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    PatchRegressionEngine::PatchRegressionEngine(PatchRegressionConfig config)
        : config_(std::move(config)) {}

    PatchRegressionScorecard PatchRegressionEngine::evaluate_patch(
        const std::string &package_id,
        const PatchMetricsSnapshot &pre_patch,
        const PatchMetricsSnapshot &post_patch) const
    {
        PatchRegressionScorecard sc{};
        sc.package_id = package_id;

        sc.neutralization_rate_pct = post_patch.attack_neutralization_rate_pct;

        // Throughput Delta %: ((Post - Pre) / Pre) * 100
        if (pre_patch.throughput_eps > 0.0)
        {
            sc.throughput_delta_pct = ((post_patch.throughput_eps - pre_patch.throughput_eps) / pre_patch.throughput_eps) * 100.0;
        }

        // Latency Delta ns: Post - Pre
        sc.latency_delta_ns = post_patch.mean_latency_ns - pre_patch.mean_latency_ns;
        sc.false_positive_delta = post_patch.false_positives_count - pre_patch.false_positives_count;

        // Verification Criteria
        bool neutralization_passed = (sc.neutralization_rate_pct >= config_.min_neutralization_pct);
        bool throughput_passed = (std::abs(sc.throughput_delta_pct) <= config_.max_throughput_drop_pct || sc.throughput_delta_pct >= 0.0);
        bool latency_passed = (sc.latency_delta_ns <= config_.max_latency_delta_ns);
        bool fp_passed = (sc.false_positive_delta == 0);

        sc.patch_approved = neutralization_passed && throughput_passed && latency_passed && fp_passed;
        sc.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "PATCH REGRESSION SCORECARD [" << package_id << "]: "
           << "Exploit Neutralization=" << sc.neutralization_rate_pct << "% (Target: " << config_.min_neutralization_pct << "%) | "
           << "Throughput Delta=" << sc.throughput_delta_pct << "% (Limit: " << config_.max_throughput_drop_pct << "%) | "
           << "Latency Delta=" << sc.latency_delta_ns << "ns (Limit: " << config_.max_latency_delta_ns << "ns) | "
           << "False Positives Delta=" << sc.false_positive_delta << " ["
           << (sc.patch_approved ? "\033[1;32m[APPROVED FOR FLEET DEPLOYMENT]\033[0m" : "\033[1;31m[REJECTED] Patch Regression Detected\033[0m") << "]";
        sc.scorecard_summary = ss.str();

        return sc;
    }

} // namespace sentinel::lab