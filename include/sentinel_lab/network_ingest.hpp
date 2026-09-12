#pragma once
#include "event.hpp"
#include "ring_buffer.hpp"
#include <thread>
#include <atomic>
#include <functional>

namespace sentinel_lab {

class NetworkIngestReceiver {
public:
    NetworkIngestReceiver(int port, LockFreeQueue& queue);
    ~NetworkIngestReceiver();

    void start();
    void stop();

    // Injects synthetic or captured batch directly into memory queue
    void inject_batch(const std::vector<BenchmarkEvent>& batch);

private:
    int port_;
    LockFreeQueue& queue_;
    std::atomic<bool> running_{false};
    std::thread listener_thread_;
};

} // namespace sentinel_lab