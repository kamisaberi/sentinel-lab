#include "sentinel_lab/network_ingest.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace sentinel_lab {

NetworkIngestReceiver::NetworkIngestReceiver(int port, LockFreeQueue& queue)
    : port_(port), queue_(queue) {}

NetworkIngestReceiver::~NetworkIngestReceiver() {
    stop();
}

void NetworkIngestReceiver::start() {
    running_ = true;
    listener_thread_ = std::thread([this]() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) return;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            return;
        }

        std::cout << "[Network Ingest] UDP Ingest Listener active on port " << port_ << std::endl;

        uint64_t counter = 0;
        char buffer[2048];

        while (running_) {
            ssize_t len = recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);
            if (len > 0) {
                counter++;
                BenchmarkEvent ev;
                ev.event_id = counter;
                ev.source_ip = "172.30.0.10";
                ev.port = 514;
                ev.t_ingest = std::chrono::high_resolution_clock::now();
                ev.features.resize(32, 0.25f);
                queue_.push(ev);
            }
        }
        close(sock);
    });
}

void NetworkIngestReceiver::stop() {
    running_ = false;
    if (listener_thread_.joinable()) {
        listener_thread_.join();
    }
}

void NetworkIngestReceiver::inject_batch(const std::vector<BenchmarkEvent>& batch) {
    for (const auto& ev : batch) {
        queue_.push(ev);
    }
}

} // namespace sentinel_lab