#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>

namespace sentinel_lab {

enum class MitigationAction {
    Passed,
    Logged,
    KernelDropped
};

struct BenchmarkEvent {
    uint64_t event_id{0};
    std::string source_ip;
    uint16_t port{0};
    std::vector<float> features; // 32 NetFlow features
    
    float anomaly_score{0.0f};
    MitigationAction action{MitigationAction::Passed};
    
    // Academic Verification: Ground Truth vs Predicted
    int ground_truth_label{-1}; // 0 = Benign, 1 = Attack, -1 = Unknown
    
    // Microsecond timing points
    std::chrono::high_resolution_clock::time_point t_ingest;
    std::chrono::high_resolution_clock::time_point t_infer_done;
    std::chrono::high_resolution_clock::time_point t_mitigated;
    double total_latency_us{0.0};
};

} // namespace sentinel_lab