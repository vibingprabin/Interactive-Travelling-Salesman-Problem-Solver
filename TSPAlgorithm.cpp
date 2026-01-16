#include "TSPAlgorithm.hpp"
#include <vector>
#include <cmath>
void computeDistanceMatrix(const std::vector<City>& cities, std::vector<std::vector<float>>& distMatrix) {
    size_t N = cities.size();
    distMatrix.assign(N, std::vector<float>(N, 0));
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            distMatrix[i][j] = (i==j) ? 1e6f : std::sqrt((cities[i].orig_x - cities[j].orig_x)*(cities[i].orig_x - cities[j].orig_x)+(cities[i].orig_y - cities[j].orig_y)*(cities[i].orig_y - cities[j].orig_y));
}
void prepareSteps(const std::vector<City>& cities, const std::vector<std::vector<float>>& distMatrix, std::vector<Step>& steps) {
    steps.clear();
    size_t N = cities.size();
    std::vector<int> assigned(N, -1);
    std::vector<bool> used(N, false);
    for (size_t i = 0; i < N; ++i) {
        Step st;
        st.assignment = assigned;
        st.current_step = i;
        st.working_row = i;
        st.working_col = -1;
        float minv = 1e9; int best = -1;
        for(size_t j = 0; j < N; ++j) {
            if (!used[j] && distMatrix[i][j] < minv) {
                minv = distMatrix[i][j];
                best = j;
            }
        }
        st.working_col = best;
        steps.push_back(st);
        if (best >= 0) { assigned[i] = best; used[best] = true; }
    }
    Step final_st = steps.back();
    final_st.assignment = assigned;
    final_st.working_row = -1; final_st.working_col = -1;
    steps.push_back(final_st);
}
