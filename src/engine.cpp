#include "sentinel_lab/engine.hpp"
#include <iostream>
#include <stdexcept>

namespace sentinel_lab {

ResearchInferenceEngine::ResearchInferenceEngine(const std::string& backend_name, const std::string& model_path)
    : backend_name_(backend_name), model_path_(model_path) {
    
    if (backend_name_ == "TensorRT") {
        target_ = xinfer::Target::TensorRT;
    } else {
        target_ = xinfer::Target::OpenVINO;
    }

    try {
        xinfer_engine_ = std::make_unique<xinfer::Engine>(target_);
        if (!model_path_.empty()) {
            load_model(model_path_);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Engine Error] Initialization failed: " << e.what() << std::endl;
        is_ready_ = false;
    }
}

bool ResearchInferenceEngine::load_model(const std::string& model_path) {
    model_path_ = model_path;
    try {
        std::cout << "[Sentinel-Lab Engine] Loading model via libxinfer.so: " << model_path_ << std::endl;
        xinfer_engine_->load_model(model_path_);
        is_ready_ = true;
        std::cout << "[Sentinel-Lab Engine] Model active on backend: " << xinfer::target_to_string(target_) << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Sentinel-Lab Engine Warning] Failed to load model: " << e.what() << std::endl;
        is_ready_ = false;
        return false;
    }
}

float ResearchInferenceEngine::predict_anomaly(const std::vector<float>& features) {
    if (!is_ready_ || features.empty()) {
        return 0.10f; // Fallback
    }

    try {
        xinfer::Tensor& input = xinfer_engine_->get_input_tensor("input");
        input.copy_from_host(features.data(), features.size() * sizeof(float));

        xinfer_engine_->infer();

        xinfer::Tensor& output = xinfer_engine_->get_output_tensor("scores");
        
        // Return attack probability (index 1 if binary classification [p_benign, p_attack])
        if (output.element_count() >= 2) {
            return output.data<float>()[1];
        }
        return output.data<float>()[0];

    } catch (...) {
        return 0.10f;
    }
}

} // namespace sentinel_lab