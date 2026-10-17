#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

namespace sentinel_lab
{

    struct PmuMetrics
    {
        bool valid{false};
        uint64_t cpu_cycles{0};
        uint64_t instructions{0};
        uint64_t llc_references{0};
        uint64_t llc_misses{0};
        uint64_t bus_cycles{0};

        double instructions_per_cycle{0.0}; // IPC
        double llc_miss_rate{0.0};          // Cache Miss Ratio [0.0 - 1.0]
        std::string status_summary;
    };

    struct PmuConfig
    {
        std::string config_path{"configs/pmu_profile.json"};
        bool enable_hw_counters{true};
        double ipc_threshold{0.5};
        double llc_miss_threshold{0.15};
    };

    class PmuProfiler
    {
    public:
        explicit PmuProfiler(PmuConfig config = {});
        ~PmuProfiler();

        /// Opens Linux perf_event_open file descriptors and enables CPU counters
        bool start();

        /// Stops hardware counters, reads final deltas, and computes microarchitectural metrics
        PmuMetrics stop();

        /// Read current hardware metrics without stopping
        [[nodiscard]] PmuMetrics read_current();

        /// Print colored terminal microarchitecture report
        static void print_pmu_report(const PmuMetrics &metrics);

        [[nodiscard]] bool is_active() const noexcept { return is_active_; }

    private:
        void close_fds();
        void load_config();

        PmuConfig config_;
        bool is_active_{false};
        std::mutex pmu_mutex_;

        int fd_cycles_{-1};
        int fd_instructions_{-1};
        int fd_llc_refs_{-1};
        int fd_llc_misses_{-1};
        int fd_bus_cycles_{-1};
    };

} // namespace sentinel_lab