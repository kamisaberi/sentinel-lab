#include "sentinel_lab/ebpf_filter.hpp"
#include <iostream>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_link.h>
#include <cstring>

namespace sentinel_lab {

EBPFFilterHarness::EBPFFilterHarness(std::string interface_name)
    : interface_name_(std::move(interface_name)) {
    ifindex_ = if_nametoindex(interface_name_.c_str());
    if (ifindex_ == 0) {
        ifindex_ = if_nametoindex("eth0");
        if (ifindex_ != 0) interface_name_ = "eth0";
    }
}

EBPFFilterHarness::~EBPFFilterHarness() {
    detach();
}

bool EBPFFilterHarness::load_and_attach(const std::string& bpf_obj_path) {
    if (ifindex_ == 0) {
        std::cerr << "[eBPF Harness] Network interface " << interface_name_ << " not found." << std::endl;
        return false;
    }

    struct bpf_object* obj = bpf_object__open_file(bpf_obj_path.c_str(), nullptr);
    if (!obj) {
        std::cerr << "[eBPF Harness] Failed to open bytecode object: " << bpf_obj_path << std::endl;
        return false;
    }

    if (bpf_object__load(obj) < 0) {
        std::cerr << "[eBPF Harness] Failed to load BPF object into kernel." << std::endl;
        bpf_object__close(obj);
        return false;
    }

    struct bpf_program* prog = bpf_object__find_program_by_name(obj, "xdp_firewall");
    prog_fd_ = bpf_program__fd(prog);
    map_fd_ = bpf_object__find_map_fd_by_name(obj, "blocked_ip_map");
    bpf_obj_ = obj;

    // Attach in XDP Generic / SKB Mode (Guarantees compatibility in VMware & Cloud VMs)
    unsigned int xdp_flags = XDP_FLAGS_SKB_MODE;
    if (bpf_xdp_attach(ifindex_, prog_fd_, xdp_flags, nullptr) < 0) {
        std::cerr << "[eBPF Harness Warning] Failed to attach XDP to " << interface_name_ << std::endl;
        return false;
    }

    std::cout << "[eBPF Harness] Native XDP filter attached to " << interface_name_ << " (SKB Mode)." << std::endl;
    return true;
}

void EBPFFilterHarness::detach() {
    if (ifindex_ > 0 && prog_fd_ > 0) {
        bpf_xdp_detach(ifindex_, XDP_FLAGS_SKB_MODE, nullptr);
        std::cout << "[eBPF Harness] Detached XDP filter from " << interface_name_ << std::endl;
        prog_fd_ = -1;
    }
    if (bpf_obj_) {
        bpf_object__close(static_cast<struct bpf_object*>(bpf_obj_));
        bpf_obj_ = nullptr;
    }
}

bool EBPFFilterHarness::block_ip(const std::string& ip_address) {
    std::lock_guard<std::mutex> lock(filter_mutex_);
    if (blocked_ips_.find(ip_address) != blocked_ips_.end()) return true;

    struct in_addr addr;
    if (inet_pton(AF_INET, ip_address.c_str(), &addr) != 1) return false;

    uint32_t key = addr.s_addr;
    uint64_t initial_count = 0;

    if (map_fd_ >= 0) {
        int res = bpf_map_update_elem(map_fd_, &key, &initial_count, BPF_ANY);
        if (res == 0) {
            blocked_ips_.insert(ip_address);
            return true;
        }
    }
    return false;
}

bool EBPFFilterHarness::unblock_ip(const std::string& ip_address) {
    std::lock_guard<std::mutex> lock(filter_mutex_);
    struct in_addr addr;
    if (inet_pton(AF_INET, ip_address.c_str(), &addr) != 1) return false;

    uint32_t key = addr.s_addr;
    if (map_fd_ >= 0) {
        bpf_map_delete_elem(map_fd_, &key);
    }
    blocked_ips_.erase(ip_address);
    return true;
}

} // namespace sentinel_lab