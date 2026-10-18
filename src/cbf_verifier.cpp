#include "sentinel_lab/cbf_verifier.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    CbfVerifier::CbfVerifier(CbfConfig config)
        : config_(std::move(config)) {}

    double CbfVerifier::evaluate_h(double temperature_c, double max_safe_temp_c) noexcept
    {
        // h(x) = max_safe_temp_c - T(t). Safe Set C = { x | h(x) >= 0 }
        return max_safe_temp_c - temperature_c;
    }

    bool CbfVerifier::verify_invariance_condition(double h_val, double h_dot_val, double alpha_gain) noexcept
    {
        // Ames/Nagumo Condition: dh/dt + alpha * h >= 0
        return (h_dot_val + (alpha_gain * h_val)) >= 0.0;
    }

    CbfVerificationResult CbfVerifier::verify_trajectory(std::span<const CbfStatePoint> trajectory) const
    {
        CbfVerificationResult res{};
        if (trajectory.empty())
        {
            res.is_formally_safe = false;
            res.verification_certificate = "Trajectory is empty";
            return res;
        }

        double min_h = 1e9;
        double min_condition = 1e9;
        size_t violations = 0;

        for (const auto &pt : trajectory)
        {
            min_h = std::min(min_h, pt.h_barrier_val);
            double cbf_cond = pt.h_dot_val + (config_.alpha_gain * pt.h_barrier_val);
            min_condition = std::min(min_condition, cbf_cond);

            if (pt.h_barrier_val < 0.0 || cbf_cond < 0.0)
            {
                violations++;
            }
        }

        res.min_barrier_margin_h = min_h;
        res.min_cbf_condition_val = min_condition;
        res.boundary_violations_count = violations;
        res.is_formally_safe = (violations == 0 && min_h >= 0.0);

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(3)
           << "FORMAL CONTROL BARRIER FUNCTION (CBF) CERTIFICATE: "
           << "Min Barrier Margin h(x)=" << res.min_barrier_margin_h << " (Safe: h >= 0) | "
           << "Min Invariance Val (dh/dt + alpha*h)=" << res.min_cbf_condition_val << " | "
           << "Boundary Violations=" << res.boundary_violations_count << " ["
           << (res.is_formally_safe ? "\033[1;32m[VERIFIED] Safe Set C Forward-Invariant\033[0m" : "\033[1;31m[UNSAFE] Barrier Invariant Breached\033[0m") << "]";
        res.verification_certificate = ss.str();

        return res;
    }

} // namespace sentinel::lab