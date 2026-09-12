#include <iostream>
#include <cassert>
#include "sentinel_lab/benchmarker.hpp"

int main() {
    std::cout << "[Test] Running Benchmarker Statistical Test..." << std::endl;

    sentinel_lab::Benchmarker benchmarker;

    // Record 100 test events with known latencies
    for (int i = 1; i <= 100; ++i) {
        sentinel_lab::BenchmarkEvent ev;
        ev.event_id = i;
        ev.total_latency_us = static_cast<double>(i); // Latencies 1 to 100 us
        ev.ground_truth_label = (i % 10 == 0) ? 1 : 0;
        ev.action = (i % 10 == 0) ? sentinel_lab::MitigationAction::KernelDropped : sentinel_lab::MitigationAction::Passed;
        benchmarker.record_event(ev);
    }

    auto m = benchmarker.compute_metrics(1.0); // 1-second duration
    
    assert(m.total_events == 100);
    assert(m.latency_min_us == 1.0);
    assert(m.latency_max_us == 100.0);
    assert(m.latency_p50_us == 51.0);
    assert(m.accuracy == 1.0); // 100% accuracy on this batch

    std::cout << "[PASS] Benchmarker Statistical Precision Test Passed." << std::endl;
    return 0;
}