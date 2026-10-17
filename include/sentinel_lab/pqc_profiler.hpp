#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    enum class PqcKemAlgorithm : uint8_t
    {
        ML_KEM_512 = 0, // Kyber-512 (800 bytes PK, 768 bytes Ciphertext)
        ML_KEM_768,     // Kyber-768 (1184 bytes PK, 1088 bytes Ciphertext)
        ML_KEM_1024     // Kyber-1024 (1568 bytes PK, 1568 bytes Ciphertext)
    };

    enum class PqcSigAlgorithm : uint8_t
    {
        ML_DSA_2 = 0, // Dilithium-2 (1312 bytes PK, 2420 bytes Signature)
        ML_DSA_3,     // Dilithium-3 (1952 bytes PK, 3293 bytes Signature)
        ML_DSA_5      // Dilithium-5 (2592 bytes PK, 4595 bytes Signature)
    };

    struct PqcOverheadMetrics
    {
        bool valid{false};
        std::string kem_name;
        std::string sig_name;
        size_t total_pqc_handshake_bytes{0};
        size_t ip_fragments_count{0};
        double reassembly_buffer_kb{0.0};
        double estimated_pqc_latency_penalty_ms{0.0};
        bool ot_sla_breached{false};
        std::string summary_report;
    };

    struct PqcConfig
    {
        std::string config_path{"configs/pqc_profile.json"};
        PqcKemAlgorithm kem{PqcKemAlgorithm::ML_KEM_768};
        PqcSigAlgorithm sig{PqcSigAlgorithm::ML_DSA_3};
        size_t mtu_bytes{1500};
        double max_ot_sla_latency_ms{1.0};
    };

    class PqcProfiler
    {
    public:
        explicit PqcProfiler(PqcConfig config = {});
        ~PqcProfiler() = default;

        /// Returns total PQC handshake key/ciphertext/signature byte overhead
        [[nodiscard]] static size_t calculate_pqc_overhead_bytes(PqcKemAlgorithm kem, PqcSigAlgorithm sig) noexcept;

        /// Evaluates IP packet fragmentation and OT sub-millisecond SLA latency impact
        [[nodiscard]] PqcOverheadMetrics evaluate_pqc_overhead(size_t raw_payload_bytes, double base_latency_ms) const;

    private:
        PqcConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using PqcProfiler = sentinel::lab::PqcProfiler;
    using PqcOverheadMetrics = sentinel::lab::PqcOverheadMetrics;
    using PqcConfig = sentinel::lab::PqcConfig;
    using PqcKemAlgorithm = sentinel::lab::PqcKemAlgorithm;
    using PqcSigAlgorithm = sentinel::lab::PqcSigAlgorithm;
}