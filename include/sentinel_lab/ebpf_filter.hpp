#pragma once
#include <string>
#include <unordered_set>
#include <mutex>

namespace sentinel_lab {

class EBPFFilterHarness {
public:
    explicit EBPFFilterHarness(std::string interface_name = "ens33");
    ~EBPFFilterHarness();

    bool load_and_attach(const std::string& bpf_obj_path);
    void detach();

    bool block_ip(const std::string& ip_address);
    bool unblock_ip(const std::string& ip_address);

    size_t get_blocked_count() const { return blocked_ips_.size(); }
    const std::unordered_set<std::string>& get_blocked_ips() const { return blocked_ips_; }

private:
    std::string interface_name_;
    int ifindex_{0};
    int map_fd_{-1};
    int prog_fd_{-1};
    void* bpf_obj_{nullptr};
    std::unordered_set<std::string> blocked_ips_;
    std::mutex filter_mutex_;
};

} // namespace sentinel_lab