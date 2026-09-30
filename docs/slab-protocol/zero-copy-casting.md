# Zero-Copy Casting

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Memory-mapping raw socket payload buffers directly to float arrays.

## Cast

Validate magic, then reinterpret offset 0x14 as const float*.

## Safety

Bounds come from the header length field, checked first.

```cpp
// After magic + bounds validation:
const float *vec =
    reinterpret_cast<const float*>(frame + 0x14);  // zero-copy
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
