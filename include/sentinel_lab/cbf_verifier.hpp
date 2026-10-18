#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    struct CbfStatePoint
    {
        double timestamp_sec{0.0};
        double temperature_c{0.0};
        double h_barrier_val{0.0};
        double h_dot_val{0.0};
        bool is_cbf_condition_satisfied{true};
    };

    struct CbfVerificationResult
    {
        bool is_formally_safe{true};
        double min_barrier_margin_h{0.0};  // Minimum h(x) value (h > 0 is inside safe set C)
        double min_cbf_condition_val{0.0}; // Minimum (dh/dt + alpha*h)
        size_t boundary_violations_count{0};
        std::string verification_certificate;
    };

    struct CbfConfig
    {
        std::string config_path{"configs/cbf_profile.json"};
        double max_safe_temp_c{120.0};
        double alpha_gain{0.5};
    };

    class CbfVerifier
    {
    public:
        explicit CbfVerifier(CbfConfig config = {});
        ~CbfVerifier() = default;

        /// Evaluates Control Barrier Function h(x) = max_temp - T(t)
        [[nodiscard]] static double evaluate_h(double temperature_c, double max_safe_temp_c) noexcept;

        /// Evaluates Nagumo's CBF Condition: dh/dt + alpha * h >= 0
        [[nodiscard]] static bool verify_invariance_condition(double h_val, double h_dot_val, double alpha_gain) noexcept;

        /// Computes formal verification scorecard across trajectory points
        [[nodiscard]] CbfVerificationResult verify_trajectory(std::span<const CbfStatePoint> trajectory) const;

    private:
        CbfConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using CbfVerifier = sentinel::lab::CbfVerifier;
    using CbfVerificationResult = sentinel::lab::CbfVerificationResult;
    using CbfStatePoint = sentinel::lab::CbfStatePoint;
    using CbfConfig = sentinel::lab::CbfConfig;
}