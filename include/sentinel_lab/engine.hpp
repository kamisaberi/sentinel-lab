#pragma once
#include <xinfer/xinfer.hpp>
#include <string>
#include <vector>
#include <memory>

namespace sentinel_lab {

class ResearchInferenceEngine {
public:
    ResearchInferenceEngine(const std::string& backend_name, const std::string& model_path);
    ~ResearchInferenceEngine() = default;

    bool load_model(const std::string& model_path);
    float predict_anomaly(const std::vector<float>& features);

    const std::string& get_backend_name() const { return backend_name_; }
    const std::string& get_model_path() const { return model_path_; }

private:
    std::string backend_name_;
    std::string model_path_;
    xinfer::Target target_;
    std::unique_ptr<xinfer::Engine> xinfer_engine_;
    bool is_ready_{false};
};

} // namespace sentinel_lab