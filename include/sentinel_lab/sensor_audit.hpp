#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct HardwareSecurityTelemetryPoint
    {
        double timestamp_sec{0.0};
        
        // Domain 01: Microarchitectural & Memory (Sensors #001 - #007)
        double instructions_per_cycle{2.42};   // #001 (IPC >= 1.0)
        double llc_miss_rate_pct{1.24};        // #002 (LLC Miss < 3.5%)
        double branch_mispredict_pct{2.10};    // #003 (Mispredict < 12%)
        uint64_t rowhammer_activations_sec{0}; // #004 (Bank Act < 500k/s)
        uint64_t bus_lock_cycles_sec{0};       // #005 (Bus Lock == 0)
        uint64_t page_faults_sec{15};          // #006 (Normal User Faults)
        
        // Domain 02: PCB, Power & Glitch (Sensors #008 - #014)
        double vcc_rail_voltage_v{3.30};       // #008 (VCC 3.3V +/- 5%)
        double clock_jitter_pct{0.02};         // #009 (Oscillator Jitter < 0.5%)
        bool chassis_tamper_detected{false};   // #010, #011 (Lid & Light Sensor)
        bool jtag_probe_attached{false};       // #012 (JTAG Clock Active)
        bool spi_flash_write_enabled{false};   // #013 (Flash Write Enable Pin)
        double puf_entropy_ratio{0.992};       // #014 (SRAM PUF Match >= 0.98)

        // Domain 08: Mechanical Kinematics (Sensor #046)
        double lvdt_rod_position_mm{150.0};    // #046 (Rod Position)
        double lvdt_drop_accel_g{0.98};        // #046 (Drop Acceleration >= 0.85g)
    };

    struct SensorAuditMetrics
    {
        bool valid{false};
        uint32_t total_sensors_audited{15};
        uint32_t passed_sensors_count{15};
        uint32_t violated_sensors_count{0};
        bool critical_hardware_breach{false};
        std::string summary_report;
    };

    struct SensorAuditConfig
    {
        std::string config_path{"configs/sensor_audit_profile.json"};
        double min_ipc{1.0};
        double max_llc_miss_pct{3.5};
        double max_rowhammer_activations_sec{500000};
        double min_vcc_voltage_v{3.135}; // 3.3V - 5%
        double max_vcc_voltage_v{3.465}; // 3.3V + 5%
    };

    class SensorAuditor
    {
    public:
        explicit SensorAuditor(SensorAuditConfig config = {});
        ~SensorAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const HardwareSecurityTelemetryPoint &sample);
        [[nodiscard]] SensorAuditMetrics evaluate_audit_metrics() const;
        void reset() noexcept;

    private:
        SensorAuditConfig config_;
        std::vector<HardwareSecurityTelemetryPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using SensorAuditor = sentinel::lab::SensorAuditor;
    using SensorAuditMetrics = sentinel::lab::SensorAuditMetrics;
    using SensorAuditConfig = sentinel::lab::SensorAuditConfig;
    using HardwareSecurityTelemetryPoint = sentinel::lab::HardwareSecurityTelemetryPoint;
}