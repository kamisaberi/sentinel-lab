#include "sentinel_lab/sentinel_lab.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>

std::atomic<bool> g_lab_running{true};

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        std::cout << "\n[Sentinel-Lab] Stopping benchmark engine..." << std::endl;
        g_lab_running = false;
    }
}

int main() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "==================================================================" << std::endl;
    std::cout << "  SENTINEL-LAB: Universal Invariant AI Evaluation Engine         " << std::endl;
    std::cout << "  Decoupled Protocol: Evaluates any model without re-compilation  " << std::endl;
    std::cout << "==================================================================" << std::endl;

    // 1. Initialize Subsystems
    sentinel_lab::LockFreeQueue queue(65536);
    sentinel_lab::Benchmarker benchmarker;
    sentinel_lab::ResearchInferenceEngine engine("OpenVINO", "models/network_threat.onnx");
    sentinel_lab::EBPFFilterHarness ebpf_harness("ens33");
    
    ebpf_harness.load_and_attach("bpf/xdp_filter.o");

    // 2. Start Generic Ingestion Receiver on Port 9000
    sentinel_lab::NetworkIngestReceiver receiver(9000, queue);
    receiver.start();

    // 3. Start Processing Worker Thread
    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t processed_counter = 0;

    std::thread worker([&]() {
        while (g_lab_running) {
            auto ev_opt = queue.pop();
            if (ev_opt.has_value()) {
                auto ev = ev_opt.value();
                processed_counter++;

                // Predict anomaly dynamically regardless of feature dimension
                ev.anomaly_score = engine.predict_anomaly(ev.features);
                ev.t_infer_done = std::chrono::high_resolution_clock::now();

                // Kernel decision logic
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

                if (processed_counter % 1000 == 0) {
                    std::cout << "[Pipeline] Evaluated " << processed_counter 
                              << " packets | Score: " << ev.anomaly_score << std::endl;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        }
    });

    std::cout << "\n[Sentinel-Lab] Engine locked and ready. Listening on UDP port 9000..." << std::endl;
    std::cout << "[Sentinel-Lab] Press Ctrl+C at any time to generate academic benchmark report.\n" << std::endl;

    while (g_lab_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_sec = std::chrono::duration<double>(end_time - start_time).count();

    receiver.stop();
    if (worker.joinable()) worker.join();

    // 4. Output Statistical Report
    auto metrics = benchmarker.compute_metrics(duration_sec);
    benchmarker.print_academic_report(metrics);
    benchmarker.export_to_csv("benchmark_results.csv");

    return 0;
}