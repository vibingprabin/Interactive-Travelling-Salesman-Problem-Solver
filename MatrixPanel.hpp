#ifndef MATRIXPANEL_HPP
#define MATRIXPANEL_HPP

#include "City.hpp"
#include "TSPAlgorithm.hpp"
#include <vector>

class MatrixPanel {
public:
    static void draw(const std::vector<City>& cities, const TSPStep& step);
};

#endif // MATRIXPANEL_HPP
