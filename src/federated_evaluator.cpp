#include "sentinel_lab/federated_evaluator.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace sentinel::lab {

FederatedEvaluator::FederatedEvaluator(FederatedConfig config)
    : config_(std::move(config)) {}

size_t FederatedEvaluator::compute_krum_selected_index(