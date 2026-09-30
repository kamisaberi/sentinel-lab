# Testbed Architecture

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Decoupled C++20 research engine and socket polling loop.

## Engine

Single-threaded poll loop; workers attach via the SPMC ring.

## Decoupling

Capture, inference, and scoring scale independently.

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
