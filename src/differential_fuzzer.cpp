#include "sentinel_lab/differential_fuzzer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>

namespace sentinel::lab
{

    DifferentialFuzzer::DifferentialFuzzer(FuzzerConfig config)
        : config_(std::move(config)) {}

    void DifferentialFuzzer::reset() noexcept {}

    std::vector<uint8_t> DifferentialFuzzer::mutate_pdu(std::span<const uint8_t> seed_pdu, MutationType strategy) const
    {
        std::vector<uint8_t> mutated(seed_pdu.begin(), seed_pdu.end());
        if (mutated.size() < 8)
            return mutated;

        static std::mt19937 gen(1337);

        switch (strategy)
        {
        case MutationType::MBAP_LENGTH_CORRUPTION:
            // Corrupt Modbus MBAP Header Length (Bytes 4-5)
            mutated[4] = static_cast<uint8_t>(gen() % 255);
            mutated[5] = static_cast<uint8_t>(gen() % 255);
            break;

        case MutationType::DUAL_FUNCTION_CODE:
            // Append second function code (FC05 Coil Write) after FC03 Read Header
            if (mutated.size() >= 12)
            {
                uint8_t fc05_override[] = {0x05, 0x00, 0x01, 0xFF, 0x00};
                mutated.insert(mutated.end(), fc05_override, fc05_override + 5);
            }
            break;

        case MutationType::COTP_HEADER_TRUNCATE:
            // Truncate S7Comm COTP Length byte
            if (mutated[0] == 0x03)
            {
                mutated[1] = static_cast<uint8_t>(gen() % 4);
            }
            break;

        case MutationType::TRAILING_PADDING:
            // Append 32 bytes of random trailing padding
            for (int i = 0; i < 32; ++i)
            {
                mutated.push_back(static_cast<uint8_t>(gen() % 256));
            }
            break;
        }

        return mutated;
    }

    DesyncVerdict DifferentialFuzzer::evaluate_oracle(
        std::span<const uint8_t> mutated_pdu,
        bool gateway_passed,
        bool plc_state_changed,
        const std::string &protocol_name,
        uint16_t port) const
    {
        DesyncVerdict verdict{};
        verdict.protocol_name = protocol_name;
        verdict.port = port;
        verdict.mutated_payload.assign(mutated_pdu.begin(), mutated_pdu.end());

        if (gateway_passed && !plc_state_changed)
        {
            verdict.classification = OracleClassification::BENIGN_NORMAL;
        }
        else if (!gateway_passed && !plc_state_changed)
        {
            verdict.classification = OracleClassification::DEFENSE_SUCCESSFUL;
        }
        else if (!gateway_passed && plc_state_changed)
        {
            verdict.classification = OracleClassification::MITIGATION_FAILURE;
        }
        else if (gateway_passed && plc_state_changed)
        {
            // PARSER DESYNC VULNERABILITY!
            verdict.desync_found = true;
            verdict.classification = OracleClassification::PARSER_DESYNC_VULNERABILITY;

            std::ostringstream ss;
            ss << "🚨 [SCADA REQUEST SMUGGLING ZERO-DAY]: Gateway passed frame as benign (V_gateway = PASS), "
               << "but target PLC executed state change (V_plc = STATE_CHANGED) on "
               << protocol_name << " (Port " << port << "). Payload Size: " << mutated_pdu.size() << " bytes.";
            verdict.vulnerability_explanation = ss.str();
        }

        return verdict;
    }

} // namespace sentinel::lab