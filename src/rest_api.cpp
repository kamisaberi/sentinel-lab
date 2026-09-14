#include "sentinel_lab/rest_api.hpp"
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

namespace sentinel_lab {

ResearchRESTServer::ResearchRESTServer(int port, Benchmarker& benchmarker, EBPFFilterHarness& filter)
    : port_(port), benchmarker_(benchmarker), filter_(filter) {}

ResearchRESTServer::~ResearchRESTServer() {
    stop();
}

void ResearchRESTServer::start() {
    running_ = true;
    server_thread_ = std::thread([this]() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) return;

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            close(server_fd);
            return;
        }

        listen(server_fd, 5);
        std::cout << "[Research API] Telemetry endpoint active at http://localhost:" << port_ << std::endl;

        while (running_) {
            int client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd >= 0) {
                char buf[1024] = {0};
                read(client_fd, buf, sizeof(buf) - 1);

                auto m = benchmarker_.compute_metrics(1.0);
                std::ostringstream json;
                json << "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n\r\n"
                     << "{\"total_events\":" << m.total_events
                     << ",\"throughput_eps\":" << m.throughput_eps
                     << ",\"mean_latency_us\":" << m.latency_mean_us
                     << ",\"p95_latency_us\":" << m.latency_p95_us
                     << ",\"p99_latency_us\":" << m.latency_p99_us
                     << ",\"blocked_ips_count\":" << filter_.get_blocked_count() << "}";

                std::string resp = json.str();
                send(client_fd, resp.c_str(), resp.size(), 0);
                close(client_fd);
            }
        }
        close(server_fd);
    });
}

void ResearchRESTServer::stop() {
    running_ = false;
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}

} // namespace sentinel_lab