#pragma once
#include <vector>
#include "City.hpp"
struct Step {
    std::vector<int> assignment;
    int current_step;
    int working_row, working_col;
};
void computeDistanceMatrix(const std::vector<City>& cities, std::vector<std::vector<float>>& distMatrix);
void prepareSteps(const std::vector<City>& cities, const std::vector<std::vector<float>>& distMatrix, std::vector<Step>& steps);
