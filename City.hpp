#pragma once
#include <vector>
#include <string>
struct City {
    float x, y;
    float orig_x, orig_y;
};
void normalizeCitiesFromRaw(std::vector<std::pair<float, float>>& raw, std::vector<City>& cities);
void loadCitiesFromJsonFile(const std::string& filename, std::vector<City>& cities);
