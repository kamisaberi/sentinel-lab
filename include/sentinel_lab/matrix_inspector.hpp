#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "sentinel_lab/evt_engine.hpp"
#include "sentinel_lab/cbf_verifier.hpp"
#include "sentinel_lab/hil_coupler.hpp"

namespace sentinel::lab
{

    struct LiveMatrixSnapshot
    {
        uint64_t timestamp_ns{0};
        uint32_t active_nodes_count{0};
        uint64_t packets_inspected{0};
        uint64_t ebpf_drops{0};
        double avg_mitigation_latency_us{0.84};
        double physical_temp_c{85.0};
        double physical_vibration_rpm{1200.0};
        double physical_pressure_psi{100.0};
    };

    struct LiveInspectionScorecard
    {
        bool valid{false};
        uint64_t total_samples_recorded{0};
        double max_observed_temp_c{0.0};
        double evt_p99_999_wcet_us{0.0};
        double min_cbf_safety_margin{0.0};
        double critical_clearing_time_ms{0.0};
        bool physical_safety_maintained{true};
        bool real_time_sla_met{true};
        std::string summary_report;
    };

    struct MatrixInspectorConfig
    {
        std::string config_path{"configs/matrix_inspector_profile.json"};
        std::string nexus_rest_url{"http://10.240.0.10:9443"};
        std::string plc_host{"10.240.0.150"};
        uint16_t plc_port{502};
        double max_safe_temp_c{120.0};
        double target_tcrit_ms{50.0};
    };

    class MatrixInspector
    {
    public:
        explicit MatrixInspector(MatrixInspectorConfig config = {});
        ~MatrixInspector() = default;

        /// Ingests a single live telemetry sample from sentinel-matrix
        void record_live_snapshot(const LiveMatrixSnapshot &snapshot);

        /// Evaluates live HIL, CBF, and EVT scorecards from the active simulation run
        [[nodiscard]] LiveInspectionScorecard evaluate_live_run() const;

        /// Reset internal state
        void reset() noexcept;

        [[nodiscard]] size_t snapshot_count() const noexcept;

    private:
        MatrixInspectorConfig config_;
        std::vector<LiveMatrixSnapshot> snapshots_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using MatrixInspector = sentinel::lab::MatrixInspector;
    using LiveMatrixSnapshot = sentinel::lab::LiveMatrixSnapshot;
    using LiveInspectionScorecard = sentinel::lab::LiveInspectionScorecard;
    using MatrixInspectorConfig = sentinel::lab::MatrixInspectorConfig;
}