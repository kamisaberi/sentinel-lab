#include "sentinel_lab/fdia_engine.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab {

FdiaEngine::FdiaEngine(FdiaConfig config)
    : config_(std::move(config)) {}

void FdiaEngine::reset() noexcept {}

FdiaTelemetryFrame FdiaEngine::generate_fdia_frame(uint64_t timestamp_ns, double current_physical_temp, bool inject_blinding) const {
    FdiaTelemetryFrame frame{};
    frame.timestamp_ns = timestamp_ns;
    frame.cyber_sensor_id = config_.cyber_sensor_id;
    frame.physical_sensor_id = config_.physical_sensor_id;
    frame.physical_true_value = current_physical_temp;
    frame.is_fdia_active = inject_blinding;

    if (inject_blinding) {
        // Spoof cyber network telemetry to claim NORMAL state (0.10) while physical drifts
        frame.cyber_reported_score = 0.10f;
    } else {
        frame.cyber_reported_score = (current_physical_temp > config_.critical_threshold) ? 0.95f : 0.10f;
    }

    return frame;
}

FdiaMetrics FdiaEngine::evaluate_causal_detection(
    std::span<const FdiaTelemetryFrame> frames,
    uint64_t detection_timestamp_ns
) const {
    FdiaMetrics m{};
    if (frames.empty()) {
        m.summary_report = "FDIA telemetry buffer is empty";
        return m;
    }

    uint64_t breach_timestamp_ns = 0;
    double max_drift = 0.0;

    for (const auto& f : frames) {
        if (f.physical_true_value > max_drift) {
            max_drift = f.physical_true_value;
        }

        if (f.physical_true_value >= config_.critical_threshold && breach_timestamp_ns == 0) {
            breach_timestamp_ns = f.timestamp_ns;
        }
    }

    m.max_physical_drift = max_drift;

    if (detection_timestamp_ns > 0 && breach_timestamp_ns > 0 && detection_timestamp_ns >= breach_timestamp_ns) {
        m.causal_alert_triggered = true;
        m.time_to_detection_ms = static_cast<double>(detection_timestamp_ns - breach_timestamp_ns) / 1e6;
        m.f1_score = 1.0;
    } else if (breach_timestamp_ns > 0 && detection_timestamp_ns == 0) {
        m.causal_alert_triggered = false;
        m.time_to_detection_ms = 999.0; // SLA breach
        m.f1_score = 0.0;
    } else {
        m.causal_alert_triggered = false;
        m.time_to_detection_ms = 0.0;
        m.f1_score = 1.0;
    }

    m.valid = true;

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2)
       << "FDIA Causal Evaluation [Signature: STUXNET_TRITON_DECEPT]: Max Physical Drift=" << m.max_physical_drift << "°C | "
       << "Threshold Limit=" << config_.critical_threshold << "°C | "
       << "T_det=" << m.time_to_detection_ms << "ms (SLA: " << config_.target_tdet_sla_ms << "ms) | "
       << "Causal Detection=" << (m.causal_alert_triggered ? "\033[1;32m[SUCCESS] Cross-Modal Interlock Triggered\033[0m" : "\033[1;31m[FAILED] Cyber Blindness Breach\033[0m");
    m.summary_report = ss.str();

    return m;
}

} // namespace sentinel::lab