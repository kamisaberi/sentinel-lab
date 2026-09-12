#pragma once
#include "event.hpp"
#include <vector>
#include <atomic>
#include <optional>
#include <cstddef>

namespace sentinel_lab {

class LockFreeQueue {
public:
    explicit LockFreeQueue(size_t capacity = 4096) : capacity_(capacity) {
        buffer_.resize(capacity_);
    }

    bool push(const BenchmarkEvent& event) {
        size_t current_tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % capacity_;

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // Queue full
        }

        buffer_[current_tail] = event;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    std::optional<BenchmarkEvent> pop() {
        size_t current_head = head_.load(std::memory_order_relaxed);

        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt; // Queue empty
        }

        BenchmarkEvent event = buffer_[current_head];
        head_.store((current_head + 1) % capacity_, std::memory_order_release);
        return event;
    }

    size_t size() const {
        size_t h = head_.load(std::memory_order_relaxed);
        size_t t = tail_.load(std::memory_order_relaxed);
        return (t >= h) ? (t - h) : (capacity_ - h + t);
    }

private:
    size_t capacity_;
    std::vector<BenchmarkEvent> buffer_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};

} // namespace sentinel_lab