#include "sentinel_lab/pqc_profiler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    PqcProfiler::PqcProfiler(PqcConfig config)
        : config_(std::move(config)) {}

    size_t PqcProfiler::calculate_pqc_overhead_bytes(PqcKemAlgorithm kem, PqcSigAlgorithm sig) noexcept
    {
        size_t kem_bytes = 0;
        size_t sig_bytes = 0;

        switch (kem)
        {
        case PqcKemAlgorithm::ML_KEM_512:
            kem_bytes = 1184 + 768;
            break;
        case PqcKemAlgorithm::ML_KEM_768:
            kem_bytes = 1184 + 1088;
            break;
        case PqcKemAlgorithm::ML_KEM_1024:
            kem_bytes = 1568 + 1568;
            break;
        }

        switch (sig)
        {
        case PqcSigAlgorithm::ML_DSA_2:
            sig_bytes = 2420;
            break;
        case PqcSigAlgorithm::ML_DSA_3:
            sig_bytes = 3293;
            break;
        case PqcSigAlgorithm::ML_DSA_5:
            sig_bytes = 4595;
            break;
        }

        return kem_bytes + sig_bytes;
    }

    PqcOverheadMetrics PqcProfiler::evaluate_pqc_overhead(size_t raw_payload_bytes, double base_latency_ms) const
    {
        PqcOverheadMetrics m{};
        m.kem_name = (config_.kem == PqcKemAlgorithm::ML_KEM_512) ? "ML-KEM-512 (Kyber)" : ((config_.kem == PqcKemAlgorithm::ML_KEM_768) ? "ML-KEM-768 (Kyber)" : "ML-KEM-1024 (Kyber)");
        m.sig_name = (config_.sig == PqcSigAlgorithm::ML_DSA_2) ? "ML-DSA-2 (Dilithium)" : ((config_.sig == PqcSigAlgorithm::ML_DSA_3) ? "ML-DSA-3 (Dilithium)" : "ML-DSA-5 (Dilithium)");

        m.total_pqc_handshake_bytes = calculate_pqc_overhead_bytes(config_.kem, config_.sig);
        size_t total_wire_bytes = raw_payload_bytes + m.total_pqc_handshake_bytes;

        // Calculate IP Packet Fragments required over MTU boundary
        m.ip_fragments_count = (total_wire_bytes + config_.mtu_bytes - 1) / config_.mtu_bytes;
        m.reassembly_buffer_kb = static_cast<double>(m.ip_fragments_count * config_.mtu_bytes) / 1024.0;

        // Estimated reassembly & PQC lattice polynomial multiplication latency penalty
        double fragmentation_penalty_ms = (m.ip_fragments_count > 1) ? (m.ip_fragments_count * 0.18) : 0.0;
        double crypto_computation_ms = 0.35; // PQC lattice polynomial solve time
        m.estimated_pqc_latency_penalty_ms = base_latency_ms + fragmentation_penalty_ms + crypto_computation_ms;

        m.ot_sla_breached = (m.estimated_pqc_latency_penalty_ms > config_.max_ot_sla_latency_ms);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "PQC Overhead Analysis [" << m.kem_name << " + " << m.sig_name << "]: "
           << "Handshake Overhead=" << m.total_pqc_handshake_bytes << " bytes | "
           << "IP Fragments=" << m.ip_fragments_count << " (MTU=" << config_.mtu_bytes << "B) | "
           << "Reassembly Buffer=" << m.reassembly_buffer_kb << "KB | "
           << "Total Latency=" << m.estimated_pqc_latency_penalty_ms << "ms (SLA Limit: " << config_.max_ot_sla_latency_ms << "ms) ["
           << (!m.ot_sla_breached ? "\033[1;32m[PASSED] OT SLA Preserved\033[0m" : "\033[1;31m[BREACH] PQC Latency Penalty Exceeds SLA\033[0m") << "]";
        m.summary_report = ss.str();

        return m;
    }

} // namespace sentinel::lab