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

    struct FederatedMetrics
    {
        bool valid{false};
        uint32_t total_nodes{5};
        uint32_t byzantine_nodes{1};
        std::string aggregation_used;
        double poisoning_attack_success_rate{0.0}; // PASR %
        double model_accuracy_loss_pct{0.0};
        double gradient_cosine_distance{0.0};
        bool byzantine_defense_successful{true};
        std::string summary_report;
    };

    struct FederatedConfig
    {
        std::string config_path{"configs/federated_profile.json"};
        uint32_t num_total_nodes{5};
        uint32_t num_byzantine_nodes{1};
        AggregationAlgorithm algorithm{AggregationAlgorithm::KRUM};
        double max_pasr_pct{2.0};
    };

    class FederatedEvaluator
    {
    public:
        explicit FederatedEvaluator(FederatedConfig config = {});
        ~FederatedEvaluator() = default;

        /// Computes Euclidean Distance between two weight gradient vectors
        [[nodiscard]] static double calculate_euclidean_distance(std::span<const float> w1, std::span<const float> w2) noexcept;

        /// Evaluates Krum / Trimmed-Mean robust aggregation metrics
        [[nodiscard]] FederatedMetrics evaluate_aggregation_robustness(
            const std::vector<std::vector<float>> &client_gradients,
            std::span<const float> true_benign_gradient,
            std::span<const bool> is_byzantine_node) const;

    private:
        FederatedConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using FederatedEvaluator = sentinel::lab::FederatedEvaluator;
    using FederatedMetrics = sentinel::lab::FederatedMetrics;
    using FederatedConfig = sentinel::lab::FederatedConfig;
    using AggregationAlgorithm = sentinel::lab::AggregationAlgorithm;
}