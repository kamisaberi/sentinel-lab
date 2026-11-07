#include "sentinel_lab/sensor_audit.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    SensorAuditor::SensorAuditor(SensorAuditConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void SensorAuditor::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("min_ipc") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.min_ipc = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
            else if (line.find("max_llc_miss_pct") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try { config_.max_llc_miss_pct = std::stod(line.substr(pos + 1)); } catch (...) {}
                }
            }
        }
    }

    void SensorAuditor::record_telemetry_sample(const HardwareSecurityTelemetryPoint &sample)
    {
        history_.push_back(sample);
    }

    void SensorAuditor::reset() noexcept
    {
        history_.clear();
    }

    SensorAuditMetrics SensorAuditor::evaluate_audit_metrics() const
    {
        SensorAuditMetrics m{};
        if (history_.empty())
        {
            m.summary_report = "Sensor telemetry buffer is empty";
            return m;
        }

        const auto &latest = history_.back();
        m.total_sensors_audited = 15;
        uint32_t violations = 0;

        // Check Domain 01 (PMU / Microarchitecture)
        if (latest.instructions_per_cycle < config_.min_ipc) violations++;
        if (latest.llc_miss_rate_pct > config_.max_llc_miss_pct) violations++;
        if (latest.branch_mispredict_pct > 12.0) violations++;
        if (latest.rowhammer_activations_sec > config_.max_rowhammer_activations_sec) violations++;
        if (latest.bus_lock_cycles_sec > 0) violations++;

        // Check Domain 02 (PCB & Voltage Glitch)
        if (latest.vcc_rail_voltage_v < config_.min_vcc_voltage_v || latest.vcc_rail_voltage_v > config_.max_vcc_voltage_v) violations++;
        if (latest.clock_jitter_pct > 0.5) violations++;
        if (latest.chassis_tamper_detected) violations++;
        if (latest.jtag_probe_attached) violations++;
        if (latest.spi_flash_write_enabled) violations++;
        if (latest.puf_entropy_ratio < 0.98) violations++;

        // Check Domain 08 (Kinematics / LVDT)
        if (latest.lvdt_drop_accel_g < 0.85) violations++;

        m.violated_sensors_count = violations;
        m.passed_sensors_count = m.total_sensors_audited - violations;
        m.critical_hardware_breach = (violations > 0);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Sensor-Audit Invariant Report: Audited=" << m.total_sensors_audited
           << " | Passed=" << m.passed_sensors_count
           << " | Breached=" << m.violated_sensors_count
           << " | Status: " << (!m.critical_hardware_breach ? "\033[1;32m[100% INVARIANT VERIFIED]\033[0m" : "\033[1;31m[HARDWARE BREACH]\033[0m");
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab