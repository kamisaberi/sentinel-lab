#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct AcousticTdoaPoint
    {
        double timestamp_sec{0.0};

        // Domain 05: Acoustic, Ultrasonic & Vibration (Sensors #029 - #034)
        double ultrasonic_airgap_kurtosis{2.95};     // #029 (Normal noise <= 3.5; FSK leak > 4.5)
        double ultrasonic_signal_power_dbm{-65.0};   // #029 (Normal ambient < -50 dBm)
        double piezo_ae_burst_energy_v2s{0.002};     // #030 (Micro-crack burst energy)
        double bearing_vibration_rms_mms{2.15};      // #031 (ISO 10816-3 limit <= 4.5 mm/s)
        double tdoa_channel_dt_us[3]{120.4, 45.2, -78.1}; // #032 (Mic 0-1, 0-2, 0-3 dt)
        double tdoa_residual_error_m{0.12};          // #032 (Localization error <= 0.50 m)
        double cavitation_acoustic_db{34.0};         // #034 (Cavitation limit <= 55 dB)

        // Domain 20: Combustion Acoustic Dynamics (Sensors #135, #140)
        double combustor_rumble_p_rms_bar{0.15};     // #135 (Dynamic pressure limit <= 0.50 bar)
        double turbopump_cavitation_rms_v{0.04};     // #140 (Cryogenic cavitation limit <= 0.15 V)

        // Domain 74: Submarine Acoustic Warfare (Sensors #521, #523)
        double towed_array_tension_kn{18.4};         // #521 (Safe tension <= 45.0 kN)
        double decoy_bubble_attenuation_db{42.0};    // #523 (Effective decoy TL >= 35.0 dB)
    };

    struct AcousticTdoaMetrics
    {
        bool valid{false};
        uint32_t total_acoustic_sensors_audited{11};
        uint32_t passed_sensors_count{11};
        uint32_t violated_sensors_count{0};
        bool acoustic_or_kinetic_breach{false};
        std::string summary_report;
    };

    struct AcousticTdoaConfig
    {
        std::string config_path{"configs/acoustic_tdoa_profile.json"};
        double max_airgap_kurtosis{3.5};
        double max_bearing_vibration_mms{4.5};
        double max_tdoa_residual_m{0.50};
        double max_combustor_rumble_bar{0.50};
        double max_towed_array_tension_kn{45.0};
        double min_decoy_attenuation_db{35.0};
    };

    class AcousticTdoaAuditor
    {
    public:
        explicit AcousticTdoaAuditor(AcousticTdoaConfig config = {});
        ~AcousticTdoaAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const AcousticTdoaPoint &sample);
        [[nodiscard]] AcousticTdoaMetrics evaluate_acoustic_metrics() const;
        void reset() noexcept;

    private:
        AcousticTdoaConfig config_;
        std::vector<AcousticTdoaPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using AcousticTdoaAuditor = sentinel::lab::AcousticTdoaAuditor;
    using AcousticTdoaMetrics = sentinel::lab::AcousticTdoaMetrics;
    using AcousticTdoaConfig = sentinel::lab::AcousticTdoaConfig;
    using AcousticTdoaPoint = sentinel::lab::AcousticTdoaPoint;
}