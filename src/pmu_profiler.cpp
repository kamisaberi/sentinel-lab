#include "sentinel_lab/pmu_profiler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

namespace sentinel_lab
{

    // Helper wrapper around Linux perf_event_open system call
    static long sys_perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                                    int cpu, int group_fd, unsigned long flags)
    {
        return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    }

    static int open_perf_counter(uint32_t type, uint64_t config)
    {
        struct perf_event_attr pe{};
        std::memset(&pe, 0, sizeof(struct perf_event_attr));
        pe.type = type;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = config;
        pe.disabled = 1;
        pe.exclude_kernel = 0; // Include kernel execution (eBPF / AF_XDP)
        pe.exclude_hv = 1;

        int fd = static_cast<int>(sys_perf_event_open(&pe, 0, -1, -1, 0));
        return fd;
    }

    PmuProfiler::PmuProfiler(PmuConfig config)
        : config_(std::move(config))
    {
        load_config();
    }

    PmuProfiler::~PmuProfiler()
    {
        close_fds();
    }

    void PmuProfiler::load_config()
    {
        std::ifstream file(config_.config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("ipc_alert_threshold") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try
                    {
                        config_.ipc_threshold = std::stod(line.substr(pos + 1));
                    }
                    catch (...)
                    {
                    }
                }
            }
            else if (line.find("llc_miss_rate_alert_threshold") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try
                    {
                        config_.llc_miss_threshold = std::stod(line.substr(pos + 1));
                    }
                    catch (...)
                    {
                    }
                }
            }
        }
    }

    void PmuProfiler::close_fds()
    {
        if (fd_cycles_ >= 0)
        {
            close(fd_cycles_);
            fd_cycles_ = -1;
        }
        if (fd_instructions_ >= 0)
        {
            close(fd_instructions_);
            fd_instructions_ = -1;
        }
        if (fd_llc_refs_ >= 0)
        {
            close(fd_llc_refs_);
            fd_llc_refs_ = -1;
        }
        if (fd_llc_misses_ >= 0)
        {
            close(fd_llc_misses_);
            fd_llc_misses_ = -1;
        }
        if (fd_bus_cycles_ >= 0)
        {
            close(fd_bus_cycles_);
            fd_bus_cycles_ = -1;
        }
    }

    bool PmuProfiler::start()
    {
        std::lock_guard<std::mutex> lock(pmu_mutex_);
        close_fds();

        fd_cycles_ = open_perf_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
        fd_instructions_ = open_perf_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
        fd_llc_refs_ = open_perf_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);
        fd_llc_misses_ = open_perf_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);
        fd_bus_cycles_ = open_perf_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BUS_CYCLES);

        if (fd_cycles_ < 0 || fd_instructions_ < 0)
        {
            std::cout << "[!] [PmuProfiler] Hardware PMU counters unavailable (VMware virtualization or kernel.perf_event_paranoid restricted)." << std::endl;
            is_active_ = false;
            close_fds();
            return false;
        }

        // Reset and enable counters
        ioctl(fd_cycles_, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd_instructions_, PERF_EVENT_IOC_RESET, 0);
        if (fd_llc_refs_ >= 0)
            ioctl(fd_llc_refs_, PERF_EVENT_IOC_RESET, 0);
        if (fd_llc_misses_ >= 0)
            ioctl(fd_llc_misses_, PERF_EVENT_IOC_RESET, 0);

        ioctl(fd_cycles_, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(fd_instructions_, PERF_EVENT_IOC_ENABLE, 0);
        if (fd_llc_refs_ >= 0)
            ioctl(fd_llc_refs_, PERF_EVENT_IOC_ENABLE, 0);
        if (fd_llc_misses_ >= 0)
            ioctl(fd_llc_misses_, PERF_EVENT_IOC_ENABLE, 0);

        is_active_ = true;
        return true;
    }

    PmuMetrics PmuProfiler::read_current()
    {
        std::lock_guard<std::mutex> lock(pmu_mutex_);
        PmuMetrics m{};
        if (!is_active_)
            return m;

        if (fd_cycles_ >= 0)
            read(fd_cycles_, &m.cpu_cycles, sizeof(uint64_t));
        if (fd_instructions_ >= 0)
            read(fd_instructions_, &m.instructions, sizeof(uint64_t));
        if (fd_llc_refs_ >= 0)
            read(fd_llc_refs_, &m.llc_references, sizeof(uint64_t));
        if (fd_llc_misses_ >= 0)
            read(fd_llc_misses_, &m.llc_misses, sizeof(uint64_t));
        if (fd_bus_cycles_ >= 0)
            read(fd_bus_cycles_, &m.bus_cycles, sizeof(uint64_t));

        m.valid = true;
        m.instructions_per_cycle = (m.cpu_cycles > 0) ? static_cast<double>(m.instructions) / m.cpu_cycles : 0.0;
        m.llc_miss_rate = (m.llc_references > 0) ? static_cast<double>(m.llc_misses) / m.llc_references : 0.0;

        return m;
    }

    PmuMetrics PmuProfiler::stop()
    {
        PmuMetrics m = read_current();
        {
            std::lock_guard<std::mutex> lock(pmu_mutex_);
            if (fd_cycles_ >= 0)
                ioctl(fd_cycles_, PERF_EVENT_IOC_DISABLE, 0);
            if (fd_instructions_ >= 0)
                ioctl(fd_instructions_, PERF_EVENT_IOC_DISABLE, 0);
            if (fd_llc_refs_ >= 0)
                ioctl(fd_llc_refs_, PERF_EVENT_IOC_DISABLE, 0);
            if (fd_llc_misses_ >= 0)
                ioctl(fd_llc_misses_, PERF_EVENT_IOC_DISABLE, 0);
            is_active_ = false;
            close_fds();
        }
        return m;
    }

    void PmuProfiler::print_pmu_report(const PmuMetrics &m)
    {
        if (!m.valid)
        {
            std::cout << "[!] PMU Microarchitecture Report: Counters unavailable" << std::endl;
            return;
        }

        std::cout << "\n------------------------------------------------------------------" << std::endl;
        std::cout << "          MICROARCHITECTURAL HARDWARE PMU PROFILER REPORT          " << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "CPU Cycles         : " << m.cpu_cycles << std::endl;
        std::cout << "Instructions       : " << m.instructions << std::endl;
        std::cout << "Instructions/Cycle : " << std::fixed << std::setprecision(3) << m.instructions_per_cycle << " IPC" << std::endl;
        std::cout << "LLC Cache Refs     : " << m.llc_references << std::endl;
        std::cout << "LLC Cache Misses   : " << m.llc_misses << std::endl;
        std::cout << "LLC Miss Rate      : " << std::setprecision(2) << (m.llc_miss_rate * 100.0) << " %" << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
    }

} // namespace sentinel_lab