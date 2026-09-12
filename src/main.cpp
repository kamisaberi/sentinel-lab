#include "sentinel_lab/sentinel_lab.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include <random>

std::atomic<bool> g_lab_running{true};

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        std::cout << "\n[Sentinel-Lab] Terminating benchmark run..." << std::endl;
        g_lab_running = false;
    }
}

// Generates synthetic benchmark streams with labeled ground truth
std::vector<sentinel_lab::BenchmarkEvent> generate_benchmark_batch(size_t count) {
    std::vector<sentinel_lab::BenchmarkEvent> batch;
    batch.reserve(count);
    std::mt19937 gen(42);
    std::normal_distribution<float> benign_dist(0.30f, 0.08f);
    std::normal_distribution<float> attack_dist(0.92f, 0.04f);

    for (size_t i = 0; i < count; ++i) {
        sentinel_lab::BenchmarkEvent ev;
        ev.event_id = i + 1;
        ev.features.resize(32);

        // 10% simulated malicious attack traffic, 90% benign
        bool is_attack = (i % 10 == 0);
        ev.ground_truth_label = is_attack ? 1 : 0;
        ev.source_ip = is_attack ? "172.30.0.250" : ("172.30.0." + std::to_string(10 + (i % 50)));
        ev.port = is_attack ? 502 : 80;

        for (size_t f = 0; f < 32; ++f) {
            ev.features[f] = is_attack ? attack_dist(gen) : benign_dist(gen);
            if (ev.features[f] < 0.0f) ev.features[f] = 0.0f;
            if (ev.features[f] > 1.0f) ev.features[f] = 1.0f;
        }
        batch.push_back(ev);
    }
    return batch;
}

int main(int argc, char** argv) {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "==================================================================" << std::endl;
    std::cout << "  SENTINEL-LAB: Academic Cyber-Physical Threat Mitigation Testbed " << std::endl;
    std::cout << "  Core Runtime: Intel OpenVINO (CPU/NPU) + Linux eBPF/XDP Hook    " << std::endl;
    std::cout << "==================================================================" << std::endl;

    // 1. Initialize Subsystems
    sentinel_lab::LockFreeQueue queue(16384);
    sentinel_lab::Benchmarker benchmarker;
    sentinel_lab::ResearchInferenceEngine engine("OpenVINO", "models/network_threat.onnx");
    sentinel_lab::EBPFFilterHarness ebpf_harness("ens33");
    
    // Attach kernel filter (if available)
    ebpf_harness.load_and_attach("bpf/xdp_filter.o");

    // 2. Start Worker Pipeline Thread
    std::thread worker([&]() {
        while (g_lab_running) {
            auto ev_opt = queue.pop();
            if (ev_opt.has_value()) {
                auto ev = ev_opt.value();

                // Inference Phase (libxinfer.so)
                ev.anomaly_score = engine.predict_anomaly(ev.features);
                ev.t_infer_done = std::chrono::high_resolution_clock::now();

                // Kernel Mitigation Decision
                if (ev.anomaly_score >= 0.85f) {
                    ebpf_harness.block_ip(ev.source_ip);
                    ev.action = sentinel_lab::MitigationAction::KernelDropped;
                } else if (ev.anomaly_score >= 0.65f) {
                    ev.action = sentinel_lab::MitigationAction::Logged;
                } else {
                    ev.action = sentinel_lab::MitigationAction::Passed;
                }

                ev.t_mitigated = std::chrono::high_resolution_clock::now();
                ev.total_latency_us = std::chrono::duration<double, std::micro>(ev.t_mitigated - ev.t_ingest).count();

                benchmarker.record_event(ev);
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
        }
    });

    // 3. Run Benchmark Suite (50,000 events)
    size_t benchmark_size = 50000;
    std::cout << "[Sentinel-Lab] Generating " << benchmark_size << " evaluation events with ground truth..." << std::endl;
    auto test_batch = generate_benchmark_batch(benchmark_size);

    std::cout << "[Sentinel-Lab] Injecting streams into pipeline..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (auto& ev : test_batch) {
        ev.t_ingest = std::chrono::high_resolution_clock::now();
        while (!queue.push(ev) && g_lab_running) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

    // Wait until queue drains
    while (queue.size() > 0 && g_lab_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_sec = std::chrono::duration<double>(end_time - start_time).count();

    // 4. Compute and Print Academic Report
    auto metrics = benchmarker.compute_metrics(duration_sec);
    benchmarker.print_academic_report(metrics);
    benchmarker.export_to_csv("benchmark_results.csv");

    g_lab_running = false;
    if (worker.joinable()) worker.join();

    std::cout << "[Sentinel-Lab] Testbed execution finished successfully." << std::endl;
    return 0;
}