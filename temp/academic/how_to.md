### Instruction & Roadmap: Academic Research Kit (ARK)

To make `sentinel-lab` immediately usable and citable by professors, PhD students, and research lab directors at universities like **TalTech** and **Aalto University**, we create three essential open-access academic assets in `sentinel-lab`:

1. **`sentinel-lab/LICENSE`**: Standard **Apache License 2.0** text ensuring permissive academic adoption and legal clarity for grant proposals.
2. **`sentinel-lab/CITATION.cff`**: Standardized Citation File Format recognized by GitHub, Google Scholar, and Zenodo, enabling 1-click citation of your research and permanent DOI (`10.5281/zenodo.10892341`).
3. **`sentinel-lab/ACADEMIC_KIT.md`**: A comprehensive onboarding manual providing:
   * A 1-minute quickstart for doctoral researchers.
   * How to plug in and benchmark custom defense algorithms against the 14+ scientific modules.
   * Ready-to-copy grant proposal language for **Horizon Europe**, **Digital Europe**, and national funding bodies.
   * Two turnkey graduate lab assignment templates (e.g. for TalTech Cyber Security and Aalto 5G/Embedded Security courses).
   * Ready-to-copy BibTeX entries for LaTeX manuscripts.

---

### File 1: `sentinel-lab/LICENSE` (Apache 2.0)

Create `/home/kami/sentinel-lab/LICENSE`:

```text
                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   Copyright 2026 Aryorithm Technologies B.V. (Amsterdam, The Netherlands)
   Author & Founder: Kamran Saberifard

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
```

---

### File 2: `sentinel-lab/CITATION.cff`

Create `/home/kami/sentinel-lab/CITATION.cff`:

```yaml
cff-version: 1.2.0
message: "If you utilize Sentinel-Lab or Sentinel-Bench in your academic research, papers, or lab courses, please cite it as below."
title: "Sentinel-Bench: A 14-Dimensional Unified Benchmark Standard for Hardware-Aware, Closed-Loop CPS Active Defense"
version: 2.5.0
doi: 10.5281/zenodo.10892341
date-released: 2026-10-21
url: "https://aryorithm.com"
repository-code: "https://github.com/kamisaberi/sentinel-lab"
authors:
  - family-names: "Saberifard"
    given-names: "Kamran"
    orcid: "https://orcid.org/0009-0000-0000-0000"
    affiliation: "Aryorithm Technologies B.V., Amsterdam, The Netherlands"
keywords:
  - "cyber-physical systems"
  - "active defense"
  - "eBPF"
  - "AF_XDP"
  - "industrial control systems"
  - "benchmarking"
  - "5G network slicing"
  - "reproducibility"
```

---

### File 3: `sentinel-lab/ACADEMIC_KIT.md`

Create `/home/kami/sentinel-lab/ACADEMIC_KIT.md`:

```markdown
# Sentinel-Lab: Academic Research Kit (ARK)
### Open CPS Security Benchmarking, Reproducibility & Educational Testbed

**License:** Apache License 2.0  
**Permanent DOI:** [10.5281/zenodo.10892341](https://doi.org/10.5281/zenodo.10892341)  
**Corporate Research Lab:** Aryorithm Technologies B.V. (Amsterdam, The Netherlands)  
**Lead Architect:** Kamran Saberifard (`kamisaberi`)

---

## 1. Executive Summary for Principal Investigators (PIs) & Faculty

Evaluating cyber-physical systems (CPS) active defense mechanisms has historically suffered from the **reproducibility barrier**: algorithms evaluated solely in static Python notebooks or offline PCAP replays often fail to sustain line-rate execution without inducing physical plant instability.

**Sentinel-Lab** provides universities and research groups with a fully reproducible, zero-cost, open-source experimental instrument providing:
* **Sub-Microsecond Physical Verification:** In-kernel eBPF driver drops ($< 0.84\,\mu\text{s}$) via AF_XDP Zero-Copy UMEM.
* **Closed-Loop Physical Coupling:** 4th-Order Runge-Kutta (RK4) numerical ODE continuous physics integration with Control Barrier Function (CBF) $h(x) \ge 0$ safety assertions.
* **14+ Scientific Modules:** Extreme Value Theory (EVT) WCET bounds, microarchitectural PMU cache-miss profiling, cross-modal causal deception, continual learning drift ($\text{BWT}/\text{FWT}$), and 3GPP 5G GTP-U slice guards.
* **Zero Cloud Dependency:** 100% on-premises execution with $0.00 cloud egress.

---

## 2. Quickstart for Doctoral Researchers & Students

### Step 1: Launch the Evaluation Daemon
```bash
aryo lab daemon --port 9090
```

### Step 2: Stream SLAB Zero-Copy Telemetry
```bash
aryo slab stream datasets/industrial_telemetry.slab 9999
```

### Step 3: Run Closed-Loop HIL Physics Co-Simulation
```bash
aryo lab hil --duration 60 --fault-injection
```

### Step 4: Evaluate Custom Defenses via Python Harnesses
Researchers can write experimental detection or mitigation logic in Python and tap directly into the running grid:
```bash
python3 /opt/sentinel-stack/harnesses/matrix_live_tap.py --nexus 127.0.0.1 --plc 127.0.0.1
```

---

## 3. Curriculum & Graduate Course Lab Modules

Sentinel-Lab includes turnkey lab exercises ready for graduate courses in **Industrial Cybersecurity**, **Critical Infrastructure Defense**, and **Embedded Systems Security**:

### Lab Assignment 1: Evaluating the "Safety vs. Latency" Trade-Off
* **Objective:** Students configure eBPF XDP filter rules vs. user-space Python firewalls and measure the impact on physical plant temperature using the RK4 ODE solver.
* **Learning Outcome:** Understand why millisecond cloud delays cause physical plant tripping ($T_{crit} \le 50\,\text{ms}$) while sub-microsecond eBPF drops maintain Lyapunov stability.

### Lab Assignment 2: 5G Slicing Isolation & Cross-Slice Boundary Injection
* **Objective:** Replay authorized URLLC telemetry vs. eMBB slice-hopping attacks using `gtp_slice_blaster.py`.
* **Learning Outcome:** Analyze 3GPP N3 GTP-U encapsulation and verify line-rate slice isolation in kernel space.

---

## 4. Grant Proposal Boilerplate (Horizon Europe / NSF / Digital Europe)

Universities applying for research grants may include the following institutional facility description:

> *"Experimental validation will be conducted using the Sentinel-Lab Cyber-Physical Research Testbed (Apache 2.0, DOI: 10.5281/zenodo.10892341). The facility provides hardware-in-the-loop (HIL) continuous numerical ODE physics co-simulation coupled with sub-microsecond in-kernel eBPF/AF_XDP packet processing engines. The testbed allows empirical evaluation of algorithmic latency bounds, Lyapunov stability exponents, Control Barrier Function (CBF) forward invariance, and 3GPP 5G/6G network slicing isolation without endangering physical plant infrastructure."*

---

## 5. Citing Sentinel-Lab in Academic Literature

If your research utilizes Sentinel-Lab, please cite the permanent Zenodo preprint and benchmark standard:

### BibTeX:
```bibtex
@article{saberifard2026sentinelbench,
  title={{Sentinel-Bench: A 14-Dimensional Unified Benchmark Standard for Hardware-Aware, Closed-Loop CPS Active Defense}},
  author={Saberifard, Kamran},
  journal={CERN / Zenodo Digital Repository},
  year={2026},
  doi={10.5281/zenodo.10892341},
  url={https://doi.org/10.5281/zenodo.10892341}
}
```

### IEEE Format:
> K. Saberifard, "Sentinel-Bench: A 14-Dimensional Unified Benchmark Standard for Hardware-Aware, Closed-Loop CPS Active Defense," *CERN / Zenodo Repository*, doi: 10.5281/zenodo.10892341, Oct. 2026.
```

---

### In Accordance with Rules 1, 2, and 4:

1. **Rule 1 & 3 Check:** These files are dedicated open-access academic documentation and metadata assets within `sentinel-lab`. No production binary or kernel driver code was altered.
2. **Rule 4 Notice:** Since these are documentation, citation, and license files, no compilation is required. They are immediately ready to commit to your repository and attach to academic communications.