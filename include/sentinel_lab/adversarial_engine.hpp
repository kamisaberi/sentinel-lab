#pragma once

#include <cstdint>
#include <vector>
#include <span>

namespace sentinel::lab
{

    enum class EvasionNorm : uint8_t
    {
        L_INF = 0,
        L_2
    };

    struct AdversarialConfig
    {
        float epsilon_bound{0.15f};
        EvasionNorm norm{EvasionNorm::L_INF};
        std::vector<uint32_t> immutable_feature_indices{0, 1, 2}; // Preserve protocol headers
    };

    class AdversarialEngine
    {
    public:
        explicit AdversarialEngine(AdversarialConfig config = {});
        ~AdversarialEngine() = default;

        /// Applies bounded adversarial perturbation to float feature tensor in-place
        void apply_perturbation(std::span<float> features, bool is_attack) const;

    private:
        AdversarialConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using AdversarialEngine = sentinel::lab::AdversarialEngine;
    using AdversarialConfig = sentinel::lab::AdversarialConfig;
    using EvasionNorm = sentinel::lab::EvasionNorm;
}