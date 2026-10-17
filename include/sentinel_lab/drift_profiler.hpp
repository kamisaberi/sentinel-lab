#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace sentinel::lab
{

    struct DriftMetrics
    {
        bool valid{false};
        size_t num_tasks{0};
        std::vector<std::vector<double>> task_matrix_R; // R[i][j]: score on task j after training on task i
        double backward_transfer_bwt{0.0};              // BWT <= 0 (negative indicates catastrophic forgetting)
        double forward_transfer_fwt{0.0};               // FWT (zero-shot transfer)
        std::vector<double> retention_decay_vector;     // Recall retention for task 0 across T_1..T_N
        bool bwt_threshold_satisfied{true};
        std::string summary_report;
    };

    struct DriftConfig
    {
        std::string config_path{"configs/drift_profile.json"};
        double bwt_min_threshold{-0.01}; // Max allowed recall drop on past tasks (1.0%)
        double fwt_min_threshold{0.00};
    };

    class DriftProfiler
    {
    public:
        explicit DriftProfiler(DriftConfig config = {});
        ~DriftProfiler() = default;

        /// Load dynamic configuration from JSON
        void load_config(const std::string &config_path);

        /// Record task matrix evaluation score R[i][j] (task_i_trained, task_j_evaluated, score)
        void record_task_score(size_t task_i, size_t task_j, double score);

        /// Computes Backward Transfer (BWT), Forward Transfer (FWT), and Retention Decay Vector
        [[nodiscard]] DriftMetrics compute_drift_metrics() const;

        /// Print colored terminal matrix report
        static void print_drift_report(const DriftMetrics &metrics);

        void reset() noexcept;

    private:
        DriftConfig config_;
        std::unordered_map<size_t, std::unordered_map<size_t, double>> matrix_raw_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using DriftProfiler = sentinel::lab::DriftProfiler;
    using DriftMetrics = sentinel::lab::DriftMetrics;
    using DriftConfig = sentinel::lab::DriftConfig;
}