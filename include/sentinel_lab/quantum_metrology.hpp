#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct QuantumMetrologyPoint
    {
        double timestamp_sec{0.0};

        // Domain 13 & 18: Quantum Spin, Qubits & Primary Metrology (#060, #121 - #127)
        double nv_diamond_dephasing_delta_khz{12.4};  // #060 (Safe limit <= 50.0 kHz)
        double transmon_qubit_readout_phase_deg{45.2}; // #121 (Nominal readout phase)
        double mkid_frequency_shift_khz{1.8};          // #122 (Single-photon kinetic shift)
        double ramsey_fringe_visibility{0.965};        // #124 (Atomic clock visibility >= 0.90)
        double tes_calorimeter_energy_ev{1.55};        // #123 (Expected telecom single photon)

        // Domain 46: Cold Atom & Quantum Gravimetry (#321, #323, #326)
        double bec_atom_interferometer_phase_rad{0.042}; // #321 (Nominal gravity phase)
        double odt_laser_power_stability_pct{0.04};     // #323 (Power drift <= 0.10%)
        double gravity_tensor_trace_eotvos{0.08};        // #326 (Laplace Invariant Tr(T) == 0, <= 0.50 E)

        // Domain 60: Superconducting Tokamak Fusion Diagnostics (#421, #423, #424, #426)
        double mirnov_tearing_mode_b_ratio_pct{0.45};   // #421 (MHD perturbation limit <= 1.50%)
        double divertor_heat_flux_mw_m2{4.2};           // #423 (Divertor safe limit <= 10.0 MW/m2)
        double plasma_current_quench_rate_ma_s{8.5};    // #424 (Quench rate limit <= 100.0 MA/s)
        double bolometer_radiated_fraction{0.48};       // #426 (Radiated power fraction <= 0.70)
    };

    struct QuantumMetrologyMetrics
    {
        bool valid{false};
        uint32_t total_quantum_sensors_audited{12};
        uint32_t passed_sensors_count{12};
        uint32_t violated_sensors_count{0};
        bool quantum_or_fusion_breach{false};
        std::string summary_report;
    };

    struct QuantumMetrologyConfig
    {
        std::string config_path{"configs/quantum_metrology_profile.json"};
        double max_nv_dephasing_khz{50.0};
        double min_ramsey_visibility{0.90};
        double max_odt_power_drift_pct{0.10};
        double max_gravity_tensor_trace_e{0.50};
        double max_mirnov_tearing_ratio_pct{1.50};
        double max_divertor_heat_flux_mw_m2{10.0};
        double max_current_quench_ma_s{100.0};
    };

    class QuantumMetrologyAuditor
    {
    public:
        explicit QuantumMetrologyAuditor(QuantumMetrologyConfig config = {});
        ~QuantumMetrologyAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const QuantumMetrologyPoint &sample);
        [[nodiscard]] QuantumMetrologyMetrics evaluate_quantum_metrics() const;
        void reset() noexcept;

    private:
        QuantumMetrologyConfig config_;
        std::vector<QuantumMetrologyPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using QuantumMetrologyAuditor = sentinel::lab::QuantumMetrologyAuditor;
    using QuantumMetrologyMetrics = sentinel::lab::QuantumMetrologyMetrics;
    using QuantumMetrologyConfig = sentinel::lab::QuantumMetrologyConfig;
    using QuantumMetrologyPoint = sentinel::lab::QuantumMetrologyPoint;
}