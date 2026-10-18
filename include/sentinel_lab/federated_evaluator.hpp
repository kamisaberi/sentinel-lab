#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    enum class AggregationAlgorithm : uint8_t
    {
        FED_AVG = 0,
        TRIMMED_MEAN,
        KRUM,
        MEDIAN
    };

    struct FederatedResilienceMetrics
    {
        bool valid{false};
        uint32_t total_nodes_N{10};
        uint32_t byzantine_nodes_K{2};
        std::string aggregation_used;
        double poisoning_attack_success_rate_pct{0.0}; // PASR %
        double benign_accuracy_retained_pct{100.0};
        double gradient_cosine_distance{0.0};
        bool byzantine_defense_passed{true};
        std::string summary_report;
    };

    struct FederatedConfig
    {
        std::string config_path{"configs/federated_profile.json"};
        uint32_t total_nodes_N{10};
        uint32_t byzantine_nodes_K{2};
        AggregationAlgorithm algorithm{AggregationAlgorithm::KRUM};
        double max_pasr_pct{1.0};
    };

    class FederatedEvaluator
    {
    public:
        explicit FederatedEvaluator(FederatedConfig config = {});
        ~FederatedEvaluator() = default;

        /// Evaluates Krum aggregation score: chooses weight vector closest to N - K - 2 neighbors
        [[nodiscard]] static size_t compute_krum_selected_index(
            const std::vector<std::vector<double>> &client_gradients,
            size_t byzantine_K) noexcept;

        /// Computes Trimmed-Mean vector across client gradients
        [[nodiscard]] static std::vector<double> compute_trimmed_mean(
            const std::vector<std::vector<double>> &client_gradients,
            double trim_ratio) noexcept;

        /// Evaluates Byzantine poisoning resilience metrics
        [[nodiscard]] FederatedResilienceMetrics evaluate_federated_resilience(
            const std::vector<std::vector<double>> &client_gradients,
            const std::vector<bool> &is_byzantine_flags,
            double benign_baseline_f1) const;

    private:
        FederatedConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using FederatedEvaluator = sentinel::lab::FederatedEvaluator;
    using FederatedResilienceMetrics = sentinel::lab::FederatedResilienceMetrics;
    using FederatedConfig = sentinel::lab::FederatedConfig;
    using AggregationAlgorithm = sentinel::lab::AggregationAlgorithm;
}