#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

namespace sentinel::lab
{

    enum class ScadaSessionState : uint8_t
    {
        UNCONNECTED = 0,
        COTP_ESTABLISHED,
        S7_SESSION_READY,
        BENIGN_POLLING,
        STEALTH_INJECTION
    };

    struct StatefulSessionFrame
    {
        ScadaSessionState current_state{ScadaSessionState::UNCONNECTED};
        uint16_t sequence_number{0};
        std::vector<uint8_t> payload;
        uint64_t delay_ms{100};
    };

    struct StatefulFuzzerConfig
    {
        std::string config_path{"configs/stateful_fuzzer_profile.json"};
        std::string protocol_name{"S7COMM"};
        uint32_t benign_polling_cycles{10};
        uint64_t inter_packet_delay_ms{100};
    };

    class StatefulFuzzer
    {
    public:
        explicit StatefulFuzzer(StatefulFuzzerConfig config = {});
        ~StatefulFuzzer() = default;

        /// Generates next PDU in the multi-step stateful SCADA session sequence
        [[nodiscard]] StatefulSessionFrame generate_next_session_frame();

        /// Resets session state back to UNCONNECTED
        void reset_session() noexcept;

        [[nodiscard]] ScadaSessionState get_current_state() const noexcept { return current_state_; }

    private:
        StatefulFuzzerConfig config_;
        ScadaSessionState current_state_{ScadaSessionState::UNCONNECTED};
        uint16_t seq_num_{0};
        uint32_t polling_count_{0};
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using StatefulFuzzer = sentinel::lab::StatefulFuzzer;
    using ScadaSessionState = sentinel::lab::ScadaSessionState;
    using StatefulSessionFrame = sentinel::lab::StatefulSessionFrame;
    using StatefulFuzzerConfig = sentinel::lab::StatefulFuzzerConfig;
}