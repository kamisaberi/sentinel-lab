#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sentinel::lab
{

    struct ChemicalGasPoint
    {
        double timestamp_sec{0.0};

        // Domain 10: Chemical, Gas & Water Sensors (#058 - #064)
        double water_ph_level{7.25};               // #061 (Safe drinking water: 6.50 - 8.50)
        double free_chlorine_residual_mg_l{1.15};  // #062 (Safe residual: 0.50 - 2.00 mg/L)
        double sf6_gas_density_kg_m3{6.12};        // #063 (Breaker dielectric minimum >= 5.00 kg/m3)
        double ambient_h2_concentration_pct{0.05}; // #064 (Flammability boundary <= 1.00%)
        double ammonia_nh3_ppm{4.2};               // #058 (IDLH limit <= 25.0 ppm)

        // Domain 23: CBRN Biosecurity & Nerve Agents (#156, #157, #162)
        double saw_nerve_agent_phase_shift_deg{0.12}; // #157 (Sarin/VX breach > 2.50 deg)
        double bio_aerosol_lif_intensity_au{18.4};    // #156 (Pathogen cloud threshold <= 50.0 AU)
        double chlorine_dioxide_mg_l{0.25};           // #162 (EPA max limit <= 0.80 mg/L)

        // Domain 33: Petrochemical Refining & Toxic Gas (#229, #233)
        double h2s_sour_gas_ppm{1.8};    // #229 (Safe limit <= 10.0 ppm)
        double hf_acid_path_ppm_m{0.15}; // #233 (Laser curtain limit <= 2.00 ppm*m)

        // Domain 79: Water Electrolyzer Gas Purity (#556, #557, #560)
        double h2_in_o2_crossover_pct{0.45};   // #556 (Explosion threshold <= 1.50%)
        double o2_in_h2_trace_ppm{1.8};        // #557 (Product purity limit <= 5.0 ppm)
        double koh_electrolyte_conc_pct{30.2}; // #560 (Normal lye: 29.0% - 31.0%)
    };

    struct ChemicalGasMetrics
    {
        bool valid{false};
        uint32_t total_chemical_sensors_audited{13};
        uint32_t passed_sensors_count{13};
        uint32_t violated_sensors_count{0};
        bool chemical_or_toxicity_breach{false};
        std::string summary_report;
    };

    struct ChemicalGasConfig
    {
        std::string config_path{"configs/chemical_gas_profile.json"};
        double min_water_ph{6.50};
        double max_water_ph{8.50};
        double min_chlorine_mg_l{0.50};
        double max_chlorine_mg_l{2.00};
        double min_sf6_density_kg_m3{5.00};
        double max_ambient_h2_pct{1.00};
        double max_h2_in_o2_pct{1.50};
        double max_h2s_ppm{10.0};
        double max_nerve_agent_phase_deg{2.50};
    };

    class ChemicalGasAuditor
    {
    public:
        explicit ChemicalGasAuditor(ChemicalGasConfig config = {});
        ~ChemicalGasAuditor() = default;

        void load_config(const std::string &config_path);
        void record_telemetry_sample(const ChemicalGasPoint &sample);
        [[nodiscard]] ChemicalGasMetrics evaluate_chemical_metrics() const;
        void reset() noexcept;

    private:
        ChemicalGasConfig config_;
        std::vector<ChemicalGasPoint> history_;
    };

} // namespace sentinel::lab

namespace sentinel_lab
{
    using ChemicalGasAuditor = sentinel::lab::ChemicalGasAuditor;
    using ChemicalGasMetrics = sentinel::lab::ChemicalGasMetrics;
    using ChemicalGasConfig = sentinel::lab::ChemicalGasConfig;
    using ChemicalGasPoint = sentinel::lab::ChemicalGasPoint;
}