#include "City.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "json.hpp"
using json = nlohmann::json;

void normalizeCitiesFromRaw(std::vector<std::pair<float, float>>& raw, std::vector<City>& cities) {
    if (raw.empty()) return;
    float minX = raw[0].first, maxX = raw[0].first;
    float minY = raw[0].second, maxY = raw[0].second;
    for (auto& pt : raw) {
        minX = std::min(minX, pt.first);
        maxX = std::max(maxX, pt.first);
        minY = std::min(minY, pt.second);
        maxY = std::max(maxY, pt.second);
    }
    float dx = (maxX - minX);
    float dy = (maxY - minY);
    if (dx == 0) dx = 1; if (dy == 0) dy = 1;
    cities.clear();
    for (auto& pt : raw) {
        float xn = (pt.first - minX) / dx;
        float yn = (pt.second - minY) / dy;
        cities.push_back( { xn*1.95f - 0.975f, yn*1.95f - 0.975f, pt.first, pt.second } );
    }
}

void loadCitiesFromJsonFile(const std::string& filename, std::vector<City>& cities) {
    std::ifstream f(filename);
    if (!f.is_open()) { std::cerr << "Failed to open JSON file: " << filename << std::endl; return; }
    try {
        json data; f >> data;
        std::vector<std::pair<float, float>> vec;
        for (const auto& item : data) vec.emplace_back(item["x"].get<float>(), item["y"].get<float>());
        normalizeCitiesFromRaw(vec, cities);
    } catch (std::exception& ex) {
        std::cerr << "Error parsing JSON: " << ex.what() << std::endl;
    }
}
