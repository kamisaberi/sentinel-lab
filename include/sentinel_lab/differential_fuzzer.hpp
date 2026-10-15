#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    enum class MutationType : uint8_t
    {
        MBAP_LENGTH_CORRUPTION = 0,
        DUAL_FUNCTION_CODE,
        COTP_HEADER_TRUNCATE,
        TRAILING_PADDING
    };

    enum class OracleClassification : uint8_t
    {
        BENIGN_NORMAL = 0,
        DEFENSE_SUCCESSFUL,
        PARSER_DESYNC_VULNERABILITY, // V_gateway == PASS && V_plc == STATE_CHANGED
        MITIGATION_FAILURE
    };

    struct DesyncVerdict
    {
        bool desync_found{false};
        OracleClassification classification{OracleClassification::BENIGN_NORMAL};
        std::string protocol_name;
        uint16_t port{0};
        std::vector<uint8_t> mutated_payload;
        std::string vulnerability_explanation;
    };

    struct FuzzerConfig
    {
        std::string config_path{"configs/fuzzer_profile.json"};
        uint32_t max_iterations{5000};
        bool log_desync_vectors{true};
    };

    class DifferentialFuzzer
    {
    public:
        explicit DifferentialFuzzer(FuzzerConfig config = {});
        ~DifferentialFuzzer() = default;

        /// Mutates raw industrial PDU according to target strategy
        [[nodiscard]] std::vector<uint8_t> mutate_pdu(std::span<const uint8_t> seed_pdu, MutationType strategy) const;

        /// Compares Gateway Verdict (V_gateway) against PLC Twin State (V_plc)
        [[nodiscard]] DesyncVerdict evaluate_oracle(
            std::span<const uint8_t> mutated_pdu,
            bool gateway_passed,
            bool plc_state_changed,
            const std::string &protocol_name,
            uint16_t port) const;

        /// Reset fuzzer state
        void reset() noexcept;

    private:
        FuzzerConfig config_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using DifferentialFuzzer = sentinel::lab::DifferentialFuzzer;
    using DesyncVerdict = sentinel::lab::DesyncVerdict;
    using OracleClassification = sentinel::lab::OracleClassification;
    using MutationType = sentinel::lab::MutationType;
}