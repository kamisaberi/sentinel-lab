#pragma once
#include "benchmarker.hpp"
#include "ebpf_filter.hpp"
#include <thread>
#include <atomic>

namespace sentinel_lab {

class ResearchRESTServer {
public:
    ResearchRESTServer(int port, Benchmarker& benchmarker, EBPFFilterHarness& filter);
    ~ResearchRESTServer();

    void start();
    void stop();

private:
    int port_;
    Benchmarker& benchmarker_;
    EBPFFilterHarness& filter_;
    std::atomic<bool> running_{false};
    std::thread server_thread_;
};

} // namespace sentinel_lab