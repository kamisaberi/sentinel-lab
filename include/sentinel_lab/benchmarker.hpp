#pragma once
#include "event.hpp"
#include <vector>
#include <string>
#include <mutex>

namespace sentinel_lab {

struct BenchmarkMetrics {
    size_t total_events{0};
    double throughput_eps{0.0};
    
    double latency_min_us{0.0};
    double latency_mean_us{0.0};
    double latency_p50_us{0.0};
    double latency_p90_us{0.0};
    double latency_p95_us{0.0};
    double latency_p99_us{0.0};
    double latency_max_us{0.0};

    // Scientific Confusion Matrix
    size_t true_positives{0};
    size_t false_positives{0};
    size_t true_negatives{0};
    size_t false_negatives{0};
    double accuracy{0.0};
    double precision{0.0};
    double recall{0.0};
    double f1_score{0.0};
};

class Benchmarker {
public:
    Benchmarker() = default;
    ~Benchmarker() = default;

    void record_event(const BenchmarkEvent& event);
    void reset();

    BenchmarkMetrics compute_metrics(double test_duration_seconds);
    void export_to_csv(const std::string& csv_path);
    void print_academic_report(const BenchmarkMetrics& m);

private:
    std::vector<double> latencies_us_;
    std::vector<BenchmarkEvent> recorded_events_;
    std::mutex bench_mutex_;
};

} // namespace sentinel_lab