#include <iostream>
#include <cassert>
#include <vector>
#include "sentinel_lab/engine.hpp"

int main() {
    std::cout << "[Test] Running Inference Engine Test..." << std::endl;
    
    sentinel_lab::ResearchInferenceEngine engine("OpenVINO", "models/network_threat.onnx");
    std::vector<float> sample_input(32, 0.25f);

    float score = engine.predict_anomaly(sample_input);
    std::cout << "Predicted Score: " << score << std::endl;

    assert(score >= 0.0f && score <= 1.0f);
    std::cout << "[PASS] Inference Engine Unit Test Passed." << std::endl;
    return 0;
}