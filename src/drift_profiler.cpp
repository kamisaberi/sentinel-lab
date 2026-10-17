#include "sentinel_lab/drift_profiler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace sentinel::lab
{

    DriftProfiler::DriftProfiler(DriftConfig config)
        : config_(std::move(config))
    {
        load_config(config_.config_path);
    }

    void DriftProfiler::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("bwt_min_threshold") != std::string::npos)
            {
                auto pos = line.find(':');
                if (pos != std::string::npos)
                {
                    try
                    {
                        config_.bwt_min_threshold = std::stod(line.substr(pos + 1));
                    }
                    catch (...)
                    {
                    }
                }
            }
        }
    }

    void DriftProfiler::record_task_score(size_t task_i, size_t task_j, double score)
    {
        matrix_raw_[task_i][task_j] = score;
    }

    void DriftProfiler::reset() noexcept
    {
        matrix_raw_.clear();
    }

    DriftMetrics DriftProfiler::compute_drift_metrics() const
    {
        DriftMetrics m{};
        if (matrix_raw_.empty())
        {
            m.summary_report = "Task matrix is empty";
            return m;
        }

        // Determine max task index N
        size_t max_idx = 0;
        for (const auto &[i, row] : matrix_raw_)
        {
            max_idx = std::max(max_idx, i);
            for (const auto &[j, val] : row)
            {
                max_idx = std::max(max_idx, j);
            }
        }
        size_t N = max_idx + 1;
        m.num_tasks = N;

        // Construct NxN Task Matrix R
        m.task_matrix_R.assign(N, std::vector<double>(N, 0.0));
        for (size_t i = 0; i < N; ++i)
        {
            for (size_t j = 0; j < N; ++j)
            {
                auto it_i = matrix_raw_.find(i);
                if (it_i != matrix_raw_.end())
                {
                    auto it_j = it_i->second.find(j);
                    if (it_j != it_i->second.end())
                    {
                        m.task_matrix_R[i][j] = it_j->second;
                    }
                }
            }
        }

        if (N < 2)
        {
            m.valid = true;
            m.summary_report = "Single task evaluated. BWT/FWT require >= 2 sequential tasks.";
            return m;
        }

        // 1. Calculate Backward Transfer (BWT)
        // BWT = (1 / (N - 1)) * sum_{j=0}^{N-2} (R[N-1][j] - R[j][j])
        double bwt_sum = 0.0;
        for (size_t j = 0; j < N - 1; ++j)
        {
            bwt_sum += (m.task_matrix_R[N - 1][j] - m.task_matrix_R[j][j]);
        }
        m.backward_transfer_bwt = bwt_sum / static_cast<double>(N - 1);

        // 2. Calculate Forward Transfer (FWT)
        // FWT = (1 / (N - 1)) * sum_{j=1}^{N-1} (R[j-1][j] - baseline_j)
        double fwt_sum = 0.0;
        for (size_t j = 1; j < N; ++j)
        {
            double baseline_j = 0.50; // Random/empty baseline assumption
            fwt_sum += (m.task_matrix_R[j - 1][j] - baseline_j);
        }
        m.forward_transfer_fwt = fwt_sum / static_cast<double>(N - 1);

        // 3. Extract Retention Decay Vector for Task 0 (Golden Attacks) across epochs
        m.retention_decay_vector.reserve(N);
        for (size_t i = 0; i < N; ++i)
        {
            m.retention_decay_vector.push_back(m.task_matrix_R[i][0]);
        }

        m.bwt_threshold_satisfied = (m.backward_transfer_bwt >= config_.bwt_min_threshold);
        m.valid = true;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(4)
           << "Continual Learning Metrics [N=" << N << " Tasks]: "
           << "Backward Transfer (BWT) = " << m.backward_transfer_bwt << " ["
           << (m.bwt_threshold_satisfied ? "NO CATASTROPHIC FORGETTING" : "FORGETTING DETECTED") << "] | "
           << "Forward Transfer (FWT) = " << m.forward_transfer_fwt;
        m.summary_report = ss.str();

        return m;
    }

    void DriftProfiler::print_drift_report(const DriftMetrics &m)
    {
        if (!m.valid)
        {
            std::cout << "[!] Drift Report: Insufficient task matrix data" << std::endl;
            return;
        }

        std::cout << "\n==================================================================" << std::endl;
        std::cout << "        SENTINEL-LAB CONTINUAL LEARNING DRIFT REPORT              " << std::endl;
        std::cout << "==================================================================" << std::endl;
        std::cout << "Evaluated Task Count  : " << m.num_tasks << std::endl;
        std::cout << "Backward Transfer BWT : " << std::fixed << std::setprecision(4) << m.backward_transfer_bwt;
        if (m.backward_transfer_bwt < 0.0)
        {
            std::cout << " (Recall Drop: " << std::setprecision(2) << (std::abs(m.backward_transfer_bwt) * 100.0) << "%)";
        }
        std::cout << std::endl;
        std::cout << "Forward Transfer FWT  : " << std::setprecision(4) << m.forward_transfer_fwt << std::endl;
        std::cout << "Safety Invariant      : " << (m.bwt_threshold_satisfied ? "\033[1;32m[PASSED] Retention Bounds Met\033[0m" : "\033[1;31m[FAILED] Catastrophic Forgetting\033[0m") << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Task Evaluation Matrix R[i][j] (Row = Trained Task, Col = Tested Task):" << std::endl;

        std::cout << "        ";
        for (size_t j = 0; j < m.num_tasks; ++j)
        {
            std::cout << "  Task_" << j;
        }
        std::cout << std::endl;

        for (size_t i = 0; i < m.num_tasks; ++i)
        {
            std::cout << "Task_" << i << " :";
            for (size_t j = 0; j < m.num_tasks; ++j)
            {
                std::cout << "  " << std::setprecision(3) << m.task_matrix_R[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << "==================================================================\n"
                  << std::endl;
    }

} // namespace sentinel::lab