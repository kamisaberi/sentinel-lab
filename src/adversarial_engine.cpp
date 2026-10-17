#include "sentinel_lab/adversarial_engine.hpp"
#include <random>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    AdversarialEngine::AdversarialEngine(AdversarialConfig config)
        : config_(std::move(config)) {}

    void AdversarialEngine::apply_perturbation(std::span<float> features, bool is_attack) const
    {
        if (!is_attack || features.empty())
            return;

        static std::mt19937 gen(1337);
        std::uniform_real_distribution<float> dis(-config_.epsilon_bound, config_.epsilon_bound);

        for (size_t i = 0; i < features.size(); ++i)
        {
            // Skip immutable protocol header feature indices
            if (std::find(config_.immutable_feature_indices.begin(),
                          config_.immutable_feature_indices.end(),
                          static_cast<uint32_t>(i)) != config_.immutable_feature_indices.end())
            {
                continue;
            }

            features[i] += dis(gen);
        }
    }

} // namespace sentinel::lab