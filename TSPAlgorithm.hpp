#ifndef TSPALGORITHM_HPP
#define TSPALGORITHM_HPP

#include "City.hpp"
#include <vector>
#include <string>
#include <utility>

// Forward declaration for Matrix template
template<class T> class Matrix;

// Structure to hold one step in the TSP solving process
struct TSPStep {
    std::vector<std::pair<int, int>> assignment;  // (from_city, to_city) pairs
    std::vector<std::vector<int>> subtours;       // Detected subtours
    std::string description;                       // Description of this step
    int iteration;                                 // Iteration number
    bool isFinalTour;                              // True if single complete tour
    
    TSPStep() : iteration(0), isFinalTour(false) {}
};

class TSPAlgorithm {
public:
    // Main solving function - returns all steps for animation
    static std::vector<TSPStep> solveWithHungarian(const std::vector<City>& cities);
    
    // Get tour length for a given assignment
    static double calculateTourLength(const std::vector<City>& cities, 
                                      const std::vector<std::pair<int, int>>& assignment);

private:
    // Build distance matrix from cities
    static void buildDistanceMatrix(const std::vector<City>& cities, 
                                     Matrix<double>& costMatrix);
    
    // Extract assignment from solved matrix (elements marked as 0)
    static std::vector<std::pair<int, int>> extractAssignment(const Matrix<double>& matrix);
    
    // Detect all subtours in the assignment
    static std::vector<std::vector<int>> findSubtours(const std::vector<std::pair<int, int>>& assignment, 
                                                       int n);
    
    // Forbid edges in subtours by setting their cost to infinity
    static void forbidSubtourEdges(Matrix<double>& costMatrix, 
                                   const std::vector<std::vector<int>>& subtours,
                                   int minSize);
};

#endif // TSPALGORITHM_HPP
