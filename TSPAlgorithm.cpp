#include "TSPAlgorithm.hpp"
#include "munkres-cpp/src/matrix.h"
#include "munkres-cpp/src/munkres.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <algorithm>

const double INF = std::numeric_limits<double>::max() / 2.0;

// Build distance matrix from city coordinates
void TSPAlgorithm::buildDistanceMatrix(const std::vector<City>& cities, 
                                        Matrix<double>& costMatrix) {
    int n = cities.size();
    costMatrix.resize(n, n, 0.0);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                costMatrix(i, j) = INF;  // No self-loops
            } else {
                // Use ORIGINAL coordinates for physical distance
                double dx = cities[i].orig_x - cities[j].orig_x;
                double dy = cities[i].orig_y - cities[j].orig_y;
                costMatrix(i, j) = std::sqrt(dx * dx + dy * dy);
            }
        }
    }
}

// Extract assignment from solved Munkres matrix
std::vector<std::pair<int, int>> TSPAlgorithm::extractAssignment(const Matrix<double>& matrix) {
    std::vector<std::pair<int, int>> assignment;
    int n = matrix.rows();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // In Munkres output, assigned cells are marked with 0
            if (matrix(i, j) == 0) {
                assignment.push_back({i, j});
                break;  // One assignment per row
            }
        }
    }
    
    return assignment;
}

// Detect all cycles/subtours in the assignment
std::vector<std::vector<int>> TSPAlgorithm::findSubtours(
    const std::vector<std::pair<int, int>>& assignment, int n) {
    
    std::vector<bool> visited(n, false);
    std::vector<std::vector<int>> subtours;
    
    // Build adjacency for quick lookup
    std::vector<int> next(n, -1);
    for (const auto& [from, to] : assignment) {
        next[from] = to;
    }
    
    // Find all cycles
    for (int start = 0; start < n; start++) {
        if (visited[start]) continue;
        
        std::vector<int> cycle;
        int current = start;
        
        // Follow the chain until we return to start
        while (!visited[current]) {
            visited[current] = true;
            cycle.push_back(current);
            current = next[current];
            
            if (current == -1) {
                std::cerr << "ERROR: Broken assignment chain!" << std::endl;
                break;
            }
        }
        
        if (!cycle.empty()) {
            subtours.push_back(cycle);
        }
    }
    
    return subtours;
}

// Forbid edges in small subtours (set cost to infinity)
void TSPAlgorithm::forbidSubtourEdges(Matrix<double>& costMatrix, 
                                      const std::vector<std::vector<int>>& subtours,
                                      int minSize) {
    for (const auto& subtour : subtours) {
        // Only forbid edges in subtours smaller than the full tour
        if (subtour.size() < minSize) {
            // Forbid the first edge in this subtour
            int from = subtour[0];
            int to = subtour[1 % subtour.size()];
            costMatrix(from, to) = INF;
            
            std::cout << "  Forbidding edge: " << from << " -> " << to 
                      << " (subtour size: " << subtour.size() << ")" << std::endl;
        }
    }
}
// Main TSP solver using Hungarian + Subtour Patching
std::vector<TSPStep> TSPAlgorithm::solveWithHungarian(const std::vector<City>& cities) {
    std::vector<TSPStep> steps;
    int n = cities.size();
    
    if (n < 2) {
        std::cerr << "Need at least 2 cities for TSP!" << std::endl;
        return steps;
    }
    
    std::cout << "\n=== Starting Hungarian TSP Solver with " << n << " cities ===" << std::endl;
    
    // Build initial cost matrix
    Matrix<double> costMatrix;
    buildDistanceMatrix(cities, costMatrix);
    
    std::cout << "Initial distance matrix built" << std::endl;
    
    int iteration = 0;
    const int MAX_ITERATIONS = 100;
    
    while (iteration < MAX_ITERATIONS) {
        std::cout << "\n--- Iteration " << iteration << " ---" << std::endl;
        
        // Make a copy for this iteration (Munkres modifies in-place)
        Matrix<double> matrixCopy = costMatrix;
        
        // Apply Hungarian algorithm
        Munkres<double> solver;
        solver.solve(matrixCopy);
        
        // Extract assignment
        auto assignment = extractAssignment(matrixCopy);
        
        std::cout << "Assignment found: " << assignment.size() << " edges" << std::endl;
        
        // Detect subtours
        auto subtours = findSubtours(assignment, n);
        
        std::cout << "Detected " << subtours.size() << " subtour(s):" << std::endl;
        for (size_t i = 0; i < subtours.size(); i++) {
            std::cout << "  Subtour " << i << " (size " << subtours[i].size() << "): ";
            for (int city : subtours[i]) {
                std::cout << city << " ";
            }
            std::cout << std::endl;
        }
        
        // Create step record
        TSPStep step;
        step.assignment = assignment;
        step.subtours = subtours;
        step.iteration = iteration;
        step.isFinalTour = (subtours.size() == 1 && subtours[0].size() == n);
        
        if (step.isFinalTour) {
            step.description = "Final Tour Found! (Single Hamiltonian Cycle)";
            std::cout << "SUCCESS: Found complete tour in iteration " << iteration << std::endl;
        } else {
            step.description = "Iteration " + std::to_string(iteration) + 
                              ": Found " + std::to_string(subtours.size()) + " subtours";
        }
        
        steps.push_back(step);
        
        // Check if we're done
        if (step.isFinalTour) {
            double tourLength = calculateTourLength(cities, assignment);
            std::cout << "Tour length: " << tourLength << std::endl;
            break;
        }
        
        // Forbid edges in subtours and continue
        forbidSubtourEdges(costMatrix, subtours, n);
        
        iteration++;
    }
    
    if (iteration >= MAX_ITERATIONS) {
        std::cerr << "WARNING: Reached maximum iterations without finding tour!" << std::endl;
    }
    
    std::cout << "\n=== Solver finished with " << steps.size() << " steps ===" << std::endl;
    
    return steps;
}

// Calculate total tour length
double TSPAlgorithm::calculateTourLength(const std::vector<City>& cities, 
                                         const std::vector<std::pair<int, int>>& assignment) {
    double total = 0.0;
    
    for (const auto& [from, to] : assignment) {
        double dx = cities[from].orig_x - cities[to].orig_x;
        double dy = cities[from].orig_y - cities[to].orig_y;
        total += std::sqrt(dx * dx + dy * dy);
    }
    
    return total;
}
