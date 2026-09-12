#include "sentinel_lab/benchmarker.hpp"
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <iomanip>

namespace sentinel_lab {

void Benchmarker::record_event(const BenchmarkEvent& event) {
    std::lock_guard<std::mutex> lock(bench_mutex_);
    latencies_us_.push_back(event.total_latency_us);
    recorded_events_.push_back(event);
}

void Benchmarker::reset() {
    std::lock_guard<std::mutex> lock(bench_mutex_);
    latencies_us_.clear();
    recorded_events_.clear();
}

BenchmarkMetrics Benchmarker::compute_metrics(double test_duration_seconds) {
    std::lock_guard<std::mutex> lock(bench_mutex_);
    BenchmarkMetrics m{};
    m.total_events = latencies_us_.size();

    if (m.total_events == 0) return m;

    m.throughput_eps = (test_duration_seconds > 0.0) 
        ? static_cast<double>(m.total_events) / test_duration_seconds 
        : 0.0;

    // Sort latencies for percentiles
    std::vector<double> sorted = latencies_us_;
    std::sort(sorted.begin(), sorted.end());

    m.latency_min_us = sorted.front();
    m.latency_max_us = sorted.back();

    double sum = std::accumulate(sorted.begin(), sorted.end(), 0.0);
    m.latency_mean_us = sum / m.total_events;

    m.latency_p50_us = sorted[static_cast<size_t>(m.total_events * 0.50)];
    m.latency_p90_us = sorted[static_cast<size_t>(m.total_events * 0.90)];
    m.latency_p95_us = sorted[static_cast<size_t>(m.total_events * 0.95)];
    m.latency_p99_us = sorted[static_cast<size_t>(m.total_events * 0.99)];

    // Evaluate Confusion Matrix against Ground Truth
    for (const auto& ev : recorded_events_) {
        if (ev.ground_truth_label == 1) {
            if (ev.action == MitigationAction::KernelDropped) m.true_positives++;
            else m.false_negatives++;
        } else if (ev.ground_truth_label == 0) {
            if (ev.action == MitigationAction::KernelDropped) m.false_positives++;
            else m.true_negatives++;
        }
    }

    size_t evaluated_samples = m.true_positives + m.false_positives + m.true_negatives + m.false_negatives;
    if (evaluated_samples > 0) {
        m.accuracy = static_cast<double>(m.true_positives + m.true_negatives) / evaluated_samples;
        m.precision = (m.true_positives + m.false_positives > 0)
            ? static_cast<double>(m.true_positives) / (m.true_positives + m.false_positives) : 1.0;
        m.recall = (m.true_positives + m.false_negatives > 0)
            ? static_cast<double>(m.true_positives) / (m.true_positives + m.false_negatives) : 1.0;
        m.f1_score = (m.precision + m.recall > 0.0) 
            ? (2.0 * m.precision * m.recall) / (m.precision + m.recall) : 0.0;
    }

    return m;
}

void Benchmarker::export_to_csv(const std::string& csv_path) {
    std::lock_guard<std::mutex> lock(bench_mutex_);
    std::ofstream f(csv_path);
    if (!f.is_open()) return;

    f << "EventID,SourceIP,Port,AnomalyScore,Action,GroundTruth,Latency_us\n";
    for (const auto& ev : recorded_events_) {
        f << ev.event_id << "," << ev.source_ip << "," << ev.port << ","
          << ev.anomaly_score << "," << static_cast<int>(ev.action) << ","
          << ev.ground_truth_label << "," << ev.total_latency_us << "\n";
    }
    std::cout << "[Benchmarker] Exported " << recorded_events_.size() << " raw evaluation points to " << csv_path << std::endl;
}

void Benchmarker::print_academic_report(const BenchmarkMetrics& m) {
    std::cout << "\n==================================================================" << std::endl;
    std::cout << "               SENTINEL-LAB ACADEMIC BENCHMARK REPORT              " << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << "Evaluated Events   : " << m.total_events << std::endl;
    std::cout << "Throughput         : " << std::fixed << std::setprecision(2) << m.throughput_eps << " EPS" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    std::cout << "Min Latency        : " << std::setprecision(2) << m.latency_min_us << " us (" << m.latency_min_us / 1000.0 << " ms)" << std::endl;
    std::cout << "Mean Latency       : " << m.latency_mean_us << " us (" << m.latency_mean_us / 1000.0 << " ms)" << std::endl;
    std::cout << "P50 Median Latency : " << m.latency_p50_us << " us" << std::endl;
    std::cout << "P95 Latency        : " << m.latency_p95_us << " us" << std::endl;
    std::cout << "P99 Latency        : " << m.latency_p99_us << " us (" << m.latency_p99_us / 1000.0 << " ms)" << std::endl;
    std::cout << "Max Latency        : " << m.latency_max_us << " us" << std::endl;
    
    if (m.true_positives + m.false_positives + m.true_negatives + m.false_negatives > 0) {
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Accuracy           : " << m.accuracy * 100.0 << " %" << std::endl;
        std::cout << "Precision          : " << m.precision * 100.0 << " %" << std::endl;
        std::cout << "Recall             : " << m.recall * 100.0 << " %" << std::endl;
        std::cout << "F1-Score           : " << m.f1_score * 100.0 << " %" << std::endl;
    }
    std::cout << "==================================================================\n" << std::endl;
}

} // namespace sentinel_lab