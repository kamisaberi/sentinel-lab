# Microsecond Clock Precision & Latency Measurement Physics

Measuring end-to-end latencies below $1.0\,\mu\text{s}$ requires nanosecond-level instrumentation. Standard operating system calls—such as `gettimeofday()` or `std::chrono::system_clock`—introduce measurement overhead ($15 - 30\,\text{ns}$) and are subject to NTP adjustments.

`sentinel-lab` uses hardware-level timestamping and CPU cycle counters.

---

## 1. Timestamping Hierarchy

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ Level 1: Hardware MAC/PHY Timestamps (PTP IEEE 1588)        │
 │  - Resolution: < 8 nanoseconds (Captured at physical layer) │
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ Level 2: Invariant Time-Stamp Counter (x86_64 TSC)          │
 │  - Instructions: __rdtscp with CPUID instruction fence      │
 │  - Resolution: ~0.33 nanoseconds (On a 3.0 GHz processor)   │
 └──────────────────────────────┬──────────────────────────────┘
                                │
 ┌──────────────────────────────┴──────────────────────────────┐
 │ Level 3: Linux Kernel CLOCK_MONOTONIC_RAW                   │
 │  - Resolution: ~15 nanoseconds (Bypasses NTP time slewing)  │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Serialized Cycle Reading Implementation

Standard `__rdtsc` instructions can be executed out-of-order by speculative CPU execution pipelines. `sentinel-lab` uses instruction-fenced cycle reading:

```cpp
#include <cstdint>

namespace sentinel::lab {

[[nodiscard]] inline uint64_t read_invariant_tsc() noexcept {
    uint32_t low, high;
    // The rdtscp instruction waits until all previous instructions have executed,
    // ensuring speculative execution does not distort the timing window.
    asm volatile("rdtscp" : "=a"(low), "=d"(high) : : "rcx");
    return (static_cast<uint64_t>(high) << 32) | low;
}

[[nodiscard]] inline double cycles_to_microseconds(uint64_t cycles, double cpu_ghz) noexcept {
    // Latency (µs) = Cycles / (CPU_Freq_GHz * 1000)
    return static_cast<double>(cycles) / (cpu_ghz * 1000.0);
}

} // namespace sentinel::lab
```

