#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    struct QuantDriftMetrics
    {
        bool valid{false};
        std::string backend_name;
        std::string precision_type; // FP16, INT8
        double cosine_similarity{1.0};
        double mean_squared_error{0.0};
        double rank_inversion_pct{0.0}; // MRD XAI attribution rank inversion %
        double avg_latency_us{0.0};
        bool precision_sla_satisfied{true};
        std::string summary_report;
    };

    struct QuantConfig
    {
        std::string config_path{"configs/quant_profile.json"};
        double min_cosine_similarity{0.985};
        double max_rank_inversion_pct{5.0};
    };

    class QuantProfiler
    {
    public:
        explicit QuantProfiler(QuantConfig config = {});
        ~QuantProfiler() = default;

        /// Calculates Cosine Similarity between FP32 baseline logits and Quantized logits
        [[nodiscard]] static double calculate_cosine_similarity(std::span<const float> fp32_logits, std::span<const float> quant_logits) noexcept;

        /// Calculates Mean Squared Error (MSE) between FP32 and Quantized outputs
        [[nodiscard]] static double calculate_mse(std::span<const float> fp32_logits, std::span<const float> quant_logits) noexcept;

        /// Calculates Spearman Rank Correlation for MRD XAI feature attribution order
        [[nodiscard]] static double calculate_rank_inversion_pct(std::span<const float> fp32_attr, std::span<const float> quant_attr) noexcept;

        /// Evaluates quantization drift metrics for a silicon target
        [[nodiscard]] QuantDriftMetrics evaluate_backend_drift(
            const std::string &backend_name,
            const std::string &precision_type,
            std::span<const float> fp32_logits,
            std::span<const float> quant_logits,
            std::span<const float> fp32_xai_attr,
            std::span<const float> quant_xai_attr,
            double latency_us) const;

    private:
        QuantConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using QuantProfiler = sentinel::lab::QuantProfiler;
    using QuantDriftMetrics = sentinel::lab::QuantDriftMetrics;
    using QuantConfig = sentinel::lab::QuantConfig;
}