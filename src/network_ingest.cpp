#include "sentinel_lab/network_ingest.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

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

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        int rcvbuf = 16 * 1024 * 1024;
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            return;
        }

        std::cout << "[Network Ingest] Generic Protocol Receiver active on port " << port_ << std::endl;

        char buffer[8192];
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        while (running_) {
            ssize_t len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);
            
            // Header must be at least 20 bytes: Magic(4B) + ID(8B) + Label(4B) + NumFeatures(4B)
            if (len >= 20) {
                char magic[5] = {0};
                std::memcpy(magic, buffer, 4);

                if (std::strcmp(magic, "SLAB") == 0) {
                    BenchmarkEvent ev;
                    ev.t_ingest = std::chrono::high_resolution_clock::now();

                    uint64_t raw_id;
                    int32_t raw_label;
                    uint32_t num_features;

                    std::memcpy(&raw_id, buffer + 4, 8);
                    std::memcpy(&raw_label, buffer + 12, 4);
                    std::memcpy(&num_features, buffer + 16, 4);

                    ev.event_id = be64toh(raw_id);
                    ev.ground_truth_label = ntohl(raw_label);
                    uint32_t n_feat = ntohl(num_features);

                    // Verify payload contains exact declared feature count
                    if (len >= static_cast<ssize_t>(20 + n_feat * sizeof(float))) {
                        ev.features.resize(n_feat);
                        std::memcpy(ev.features.data(), buffer + 20, n_feat * sizeof(float));

                        char ip_str[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
                        ev.source_ip = ip_str;
                        ev.port = ntohs(client_addr.sin_port);

                        queue_.push(ev);
                    }
                }
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