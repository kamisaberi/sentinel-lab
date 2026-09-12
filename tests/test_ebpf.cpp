#include <iostream>
#include <cassert>
#include "sentinel_lab/ebpf_filter.hpp"

int main() {
    std::cout << "[Test] Running eBPF Harness Test..." << std::endl;

    sentinel_lab::EBPFFilterHarness harness("ens33");
    
    std::string test_ip = "172.30.0.250";
    harness.block_ip(test_ip);
    assert(harness.get_blocked_count() == 1);

    harness.unblock_ip(test_ip);
    assert(harness.get_blocked_count() == 0);

    std::cout << "[PASS] eBPF Filter Unit Test Passed." << std::endl;
    return 0;
}