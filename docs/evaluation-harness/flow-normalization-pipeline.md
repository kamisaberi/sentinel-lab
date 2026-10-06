# Flow Normalization Pipeline & Continuous Tensor Clamping

Raw network flows contain wide-ranging numerical scales (e.g., flow durations ranging from $10^{-6}$ to $10^8\,\mu\text{s}$, while TCP flag counts range from $0$ to $1$). Feeding unscaled values into edge AI silicon leads to numerical overflow and gradient instability.

`sentinel-lab` normalizes features into bounded floating-point tensors spanning **$[-1.0, 1.0]$**.

---

## 1. Feature Transformation Mathematics

For each numerical feature dimension $j \in \{1 \dots 32\}$:

### 1. Robust Range Scaling
For unbounded continuous metrics (e.g., `Flow Duration`, `Total Bytes`, `Flow Packets/s`), logarithmic pre-scaling is applied:

$$x'_j = \ln(1.0 + |x_j|)$$

### 2. Min-Max Mapping to $[-1.0, 1.0]$

$$x_{\text{norm}, j} = 2.0 \cdot \left(\frac{x'_j - \min_j}{\max_j - \min_j}\right) - 1.0$$

Where $\min_j$ and $\max_j$ represent empirical baseline bounds calculated from normal training traffic.

### 3. Numerical Clamping & NaN Replacement
To guarantee that edge inferencing never receives invalid floating-point values:

$$x_{\text{final}, j} = \begin{cases} -1.0, & \text{if } x_{\text{norm}, j} < -1.0 \\ 1.0, & \text{if } x_{\text{norm}, j} > 1.0 \\ 0.0, & \text{if } x_j \text{ is NaN or } \infty \\ x_{\text{norm}, j}, & \text{otherwise} \end{cases}$$

---

## 2. Ingestion Transformation Matrix

```python
import numpy as np

def normalize_flow_matrix(raw_matrix: np.ndarray, mins: np.ndarray, maxs: np.ndarray) -> np.ndarray:
    """Vectorized transformation of raw CSV flow records to bounded float32 tensors."""
    # Apply log1p scaling to continuous flow metrics
    log_scaled = np.log1p(np.abs(raw_matrix))
    
    # MinMax scaling into [-1.0, 1.0]
    denom = np.where((maxs - mins) == 0, 1.0, (maxs - mins))
    normalized = 2.0 * ((log_scaled - mins) / denom) - 1.0
    
    # Enforce clamping and zero out NaNs
    clamped = np.clip(normalized, -1.0, 1.0)
    clamped = np.nan_to_num(clamped, nan=0.0, posinf=1.0, neginf=-1.0)
    
    return clamped.astype(np.float32)
```

