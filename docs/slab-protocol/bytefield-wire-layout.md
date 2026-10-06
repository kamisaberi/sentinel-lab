# Formal Bytefield Wire Layout & C++20 Data Structure

To ensure zero-copy deserialization across heterogeneous CPU architectures (x86_64, aarch64), the SLAB header is explicitly packed to 64-bit alignment boundaries.

---

## 1. C++20 Header Structure (`include/sentinel_lab/slab_protocol.hpp`)

```cpp
#pragma once

#include <cstdint>
#include <span>

namespace sentinel::lab {

// 0x534C4142 corresponds to ASCII: 'S', 'L', 'A', 'B'
inline constexpr uint32_t SLAB_MAGIC = 0x534C4142;

#pragma pack(push, 1)
struct SlabHeader {
    uint32_t magic;          // Byte 0-3  : 0x534C4142
    uint64_t event_id;       // Byte 4-11 : Sequence Number
    uint32_t ground_truth;   // Byte 12-15: 0=Benign, 1=Malicious
    uint32_t dimensions;     // Byte 16-19: Feature count D
    uint32_t flags;          // Byte 20-23: Bitmask flags / Reserved
};
#pragma pack(pop)

static_assert(sizeof(SlabHeader) == 24, "SlabHeader must be exactly 24 bytes");

} // namespace sentinel::lab
```

---

## 2. Hexadecimal Wire Inspection

An example hex dump of a raw SLAB packet received on an `AF_PACKET` socket:

```text
Offset    00 01 02 03  04 05 06 07  08 09 0A 0B  0C 0D 0E 0F   ASCII
-------------------------------------------------------------------------
00000000  53 4C 41 42  01 00 00 00  00 00 00 00  01 00 00 00   SLAB............
          [  Magic  ]  [      Event ID: 1     ]  [ Class: 1  ]

00000010  20 00 00 00  00 00 00 00  3D 0A D7 A3  41 40 00 00    .......=...A@..
          [ Dim: 32 ]  [ Reserved ]  [ Float 0 ]  [ Float 1 ]
```

* Bytes `00-03` (`53 4C 41 42`): Validates the frame as a legitimate research packet.
* Bytes `04-0B` (`01 00 00 00 00 00 00 00`): Sequence identifier `1`.
* Bytes `0C-0F` (`01 00 00 00`): Ground-truth annotation `1` (Attack).
* Bytes `10-13` (`20 00 00 00`): `0x20` = 32 dimensions follow.

