# Scientific Metrics Calculation & Confusion Matrix Derivation

`sentinel-lab` evaluates edge intrusion detection models using standardized statistical classification metrics, comparing in-kernel mitigation verdicts against the embedded SLAB ground-truth label.

---

## 1. Confusion Matrix Definitions

```text
                              PREDICTED CLASS
                       Benign (0)         Attack (1)
                   ┌──────────────────┬──────────────────┐
        Benign (0) │  True Negative   │  False Positive  │
                   │       (TN)       │       (FP)       │
 TRUE              ├──────────────────┼──────────────────┤
 CLASS  Attack (1) │  False Negative  │  True Positive   │
                   │       (FN)       │       (TP)       │
                   └──────────────────┴──────────────────┘
```

* **True Positive (TP):** Malicious flow correctly identified and dropped by in-kernel eBPF filter.
* **False Positive (FP):** Benign flow incorrectly dropped (Critical operational error in industrial OT).
* **False Negative (FN):** Malicious flow missed by model and allowed to pass into the network stack.
* **True Negative (TN):** Clean flow correctly forwarded via `XDP_PASS`.

---

## 2. Statistical Metric Formulations

$$\text{Accuracy} = \frac{TP + TN}{TP + TN + FP + FN}$$

$$\text{Precision} = \frac{TP}{TP + FP}$$

$$\text{Recall (Sensitivity)} = \frac{TP}{TP + FN}$$

$$F_1\text{-Score} = 2 \cdot \frac{\text{Precision} \cdot \text{Recall}}{\text{Precision} + \text{Recall}} = \frac{2 \cdot TP}{2 \cdot TP + FP + FN}$$

---

## 3. C++20 Evaluation Implementation (`src/metrics/evaluator.cpp`)

```cpp
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace sentinel::lab {

struct ScientificMetrics {
    uint64_t tp{0};
    uint64_t fp{0};
    uint64_t tn{0};
    uint64_t fn{0};

    void record_event(uint32_t ground_truth, bool predicted_attack) noexcept {
        if (ground_truth == 1 && predicted_attack) {
            tp++;
        } else if (ground_truth == 0 && predicted_attack) {
            fp++;
        } else if (ground_truth == 0 && !predicted_attack) {
            tn++;
        } else if (ground_truth == 1 && !predicted_attack) {
            fn++;
        }
    }

    [[nodiscard]] double accuracy() const noexcept {
        uint64_t total = tp + tn + fp + fn;
        return total > 0 ? static_cast<double>(tp + tn) / total : 0.0;
    }

    [[nodiscard]] double precision() const noexcept {
        uint64_t denom = tp + fp;
        return denom > 0 ? static_cast<double>(tp) / denom : 0.0;
    }

    [[nodiscard]] double recall() const noexcept {
        uint64_t denom = tp + fn;
        return denom > 0 ? static_cast<double>(tp) / denom : 0.0;
    }

    [[nodiscard]] double f1_score() const noexcept {
        double p = precision();
        double r = recall();
        return (p + r) > 0.0 ? (2.0 * p * r) / (p + r) : 0.0;
    }
};

} // namespace sentinel::lab
```

