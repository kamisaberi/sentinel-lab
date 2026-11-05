# Architecture & Low-Level Design

`sentinel-lab` is engineered as an empirical testbed daemon written in ISO C++20, decoupling socket reception from AI evaluation and metric recording.

## Section Navigation

- [Testbed Architecture](./testbed-architecture.md): Engine subsystems, busy-polling raw socket loops, and metrics pipeline.
- [Reproducibility Imperative](./academic-reproducibility-imperative.md): Addressing the academic reproducibility crisis and the "CSV Illusion".
- [Hardware-in-the-Loop Design](./hardware-in-the-loop-design.md): Bridging theoretical algorithms and physical network hardware.
- [Latency Measurement Physics](./latency-measurement-physics.md): Nanosecond clock precision, x86_64 TSC cycle counters, and PTP IEEE 1588.
- [Zero-Overhead Instrumentation](./zero-overhead-instrumentation.md): Pre-allocated logarithmic histogram bins eliminating observer effect distortions.
