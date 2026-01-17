#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include "json.hpp"
#include "City.hpp"
#include "TSPAlgorithm.hpp"
#include "MatrixPanel.hpp"
#include "RenderUtils.hpp"

using json = nlohmann::json;

// Global state
std::vector<City> cities;
std::vector<TSPStep> tspSteps;
int currentStepIndex = -1;
bool showMatrix = true;

// Track current window size
int winWidth = 800, winHeight = 600;

// Salesman animation
float salesmanProgress = 0.0f;
int salesmanCurrentEdge = 0;
bool salesmanAnimating = false;
unsigned int lastTime = 0;
float animationSpeed = 0.005f;  // Default: slow and smooth
bool fastMode = false;

// Load cities from JSON
void loadCitiesFromJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << std::endl;
        return;
    }
    
    json j;
    file >> j;
    
    cities.clear();
    
    // Support both formats:
    // 1. Raw array: [ {"x": ..., "y": ...}, ... ]
    // 2. Object with cities key: { "cities": [ ... ] }
    json& cityArray = j.is_array() ? j : j["cities"];
    
    int index = 0;
    for (const auto& cityData : cityArray) {
        City c;
        // Use name if provided, otherwise generate one
        if (cityData.contains("name")) {
            c.name = cityData["name"];
        } else {
            c.name = "City" + std::to_string(index);
        }
        c.orig_x = cityData["x"];
        c.orig_y = cityData["y"];
        c.x = c.orig_x;
        c.y = c.orig_y;
        cities.push_back(c);
        index++;
    }
    
    std::cout << "Loaded " << cities.size() << " cities from " << filename << std::endl;
}

// Solve TSP and prepare animation steps
void solveTSP() {
    if (cities.size() < 2) {
        std::cout << "Need at least 2 cities to solve TSP" << std::endl;
        return;
    }
    
    std::cout << "\nSolving TSP..." << std::endl;
    tspSteps = TSPAlgorithm::solveWithHungarian(cities);
    currentStepIndex = 0;
    
    std::cout << "Solution complete: " << tspSteps.size() << " steps generated" << std::endl;
    salesmanAnimating = false;
    salesmanProgress = 0.0f;
    salesmanCurrentEdge = 0;
    glutPostRedisplay();
}

// Draw a pixelated salesman sprite
void drawSalesman(float x, float y) {
    float px = 0.008f;  // Pixel size
    
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    
    // Head (skin tone) - centered
    glColor3f(0.96f, 0.80f, 0.65f);
    glVertex2f(x, y + px * 5);
    glVertex2f(x - px, y + px * 5);
    glVertex2f(x + px, y + px * 5);
    glVertex2f(x, y + px * 4);
    
    // Eyes (black dots)
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(x - px * 0.7f, y + px * 5);
    glVertex2f(x + px * 0.7f, y + px * 5);
    
    // Hat (brown)
    glColor3f(0.3f, 0.2f, 0.1f);
    glVertex2f(x - px * 1.5f, y + px * 6);
    glVertex2f(x - px * 0.5f, y + px * 6);
    glVertex2f(x + px * 0.5f, y + px * 6);
    glVertex2f(x + px * 1.5f, y + px * 6);
    glVertex2f(x, y + px * 7);
    
    // Neck/Collar (white)
    glColor3f(0.95f, 0.95f, 0.95f);
    glVertex2f(x, y + px * 3);
    
    // Tie (red)
    glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(x, y + px * 2);
    glVertex2f(x, y + px * 1);
    
    // Body - Shirt (white)
    glColor3f(0.95f, 0.95f, 0.95f);
    glVertex2f(x - px, y + px * 2);
    glVertex2f(x + px, y + px * 2);
    
    // Jacket (dark blue)
    glColor3f(0.1f, 0.15f, 0.35f);
    glVertex2f(x - px * 2, y + px * 2);
    glVertex2f(x + px * 2, y + px * 2);
    glVertex2f(x - px * 2, y + px * 1);
    glVertex2f(x + px * 2, y + px * 1);
    glVertex2f(x - px * 2, y);
    glVertex2f(x + px * 2, y);
    
    // Left Arm
    glColor3f(0.1f, 0.15f, 0.35f);
    glVertex2f(x - px * 3, y + px * 2);
    glVertex2f(x - px * 3, y + px * 1);
    glVertex2f(x - px * 3, y);
    // Left Hand (skin)
    glColor3f(0.96f, 0.80f, 0.65f);
    glVertex2f(x - px * 3, y - px);
    
    // Right Arm
    glColor3f(0.1f, 0.15f, 0.35f);
    glVertex2f(x + px * 3, y + px * 2);
    glVertex2f(x + px * 3, y + px * 1);
    glVertex2f(x + px * 3, y);
    // Right Hand (skin)
    glColor3f(0.96f, 0.80f, 0.65f);
    glVertex2f(x + px * 3, y - px);
    
    // Briefcase (brown) - attached to right hand
    glColor3f(0.55f, 0.35f, 0.20f);
    glVertex2f(x + px * 3.5f, y - px);
    glVertex2f(x + px * 4, y - px);
    glVertex2f(x + px * 3.5f, y - px * 2);
    glVertex2f(x + px * 4, y - px * 2);
    glVertex2f(x + px * 3.5f, y - px * 3);
    glVertex2f(x + px * 4, y - px * 3);
    // Handle (black)
    glColor3f(0.1f, 0.1f, 0.1f);
    glVertex2f(x + px * 3.7f, y);
    
    // Pants (gray)
    glColor3f(0.25f, 0.25f, 0.30f);
    glVertex2f(x - px, y - px);
    glVertex2f(x + px, y - px);
    glVertex2f(x - px, y - px * 2);
    glVertex2f(x + px, y - px * 2);
    
    // Left Leg
    glVertex2f(x - px, y - px * 3);
    glVertex2f(x - px, y - px * 4);
    // Right Leg  
    glVertex2f(x + px, y - px * 3);
    glVertex2f(x + px, y - px * 4);
    
    // Shoes (black)
    glColor3f(0.1f, 0.1f, 0.1f);
    glVertex2f(x - px * 1.5f, y - px * 5);
    glVertex2f(x - px * 0.5f, y - px * 5);
    glVertex2f(x + px * 0.5f, y - px * 5);
    glVertex2f(x + px * 1.5f, y - px * 5);
    
    glEnd();
}

// Animation timer callback
void timer(int value) {
    if (salesmanAnimating && currentStepIndex >= 0 && currentStepIndex < tspSteps.size()) {
        const TSPStep& step = tspSteps[currentStepIndex];
        
        if (step.isFinalTour && !step.subtours.empty()) {
            salesmanProgress += animationSpeed;  // Variable speed
            
            if (salesmanProgress >= 1.0f) {
                salesmanProgress = 0.0f;
                salesmanCurrentEdge++;
                
                if (salesmanCurrentEdge >= step.subtours[0].size()) {
                    salesmanCurrentEdge = 0;  // Loop back to start
                }
            }
            
            glutPostRedisplay();
            glutTimerFunc(16, timer, 0);  // ~60 FPS
        }
    }
}

// Rendering
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw cities
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (const auto& city : cities) {
        float normX = (city.x / (float)winWidth) * 2.0f - 1.0f;
        float normY = 1.0f - (city.y / (float)winHeight) * 2.0f;
        glVertex2f(normX, normY);
    }
    glEnd();
    
    // Draw city labels
    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < cities.size(); i++) {
        float normX = (cities[i].x / (float)winWidth) * 2.0f - 1.0f;
        float normY = 1.0f - (cities[i].y / (float)winHeight) * 2.0f;
        RenderUtils::drawText(normX + 0.02f, normY + 0.02f, std::to_string(i).c_str());
    }
    
    // Draw current step's assignment
    if (currentStepIndex >= 0 && currentStepIndex < tspSteps.size()) {
        const TSPStep& step = tspSteps[currentStepIndex];
        
        // Draw edges with different colors for different subtours
        const float colors[][3] = {
            {0.0f, 1.0f, 0.0f},  // Green
            {1.0f, 0.0f, 0.0f},  // Red
            {1.0f, 1.0f, 0.0f},  // Yellow
            {1.0f, 0.0f, 1.0f},  // Magenta
            {0.0f, 1.0f, 1.0f},  // Cyan
        };
        
        for (size_t s = 0; s < step.subtours.size(); s++) {
            const auto& subtour = step.subtours[s];
            int colorIdx = s % 5;
            glColor3fv(colors[colorIdx]);
            
            glLineWidth(step.isFinalTour ? 3.0f : 2.0f);
            glBegin(GL_LINE_LOOP);
            for (int cityIdx : subtour) {
                float normX = (cities[cityIdx].x / (float)winWidth) * 2.0f - 1.0f;
                float normY = 1.0f - (cities[cityIdx].y / (float)winHeight) * 2.0f;
                glVertex2f(normX, normY);
            }
            glEnd();
        }
        
        // Draw animated salesman on final tour
        if (step.isFinalTour && salesmanAnimating && !step.subtours.empty()) {
            const auto& tour = step.subtours[0];
            
            if (tour.size() > 1) {
                int fromIdx = tour[salesmanCurrentEdge];
                int toIdx = tour[(salesmanCurrentEdge + 1) % tour.size()];
                
                float fromX = (cities[fromIdx].x / (float)winWidth) * 2.0f - 1.0f;
                float fromY = 1.0f - (cities[fromIdx].y / (float)winHeight) * 2.0f;
                float toX = (cities[toIdx].x / (float)winWidth) * 2.0f - 1.0f;
                float toY = 1.0f - (cities[toIdx].y / (float)winHeight) * 2.0f;
                
                float salesmanX = fromX + (toX - fromX) * salesmanProgress;
                float salesmanY = fromY + (toY - fromY) * salesmanProgress;
                
                drawSalesman(salesmanX, salesmanY);
            }
        }
        
        // Draw info text
        glColor3f(1.0f, 1.0f, 1.0f);
        std::string info = "Step " + std::to_string(currentStepIndex + 1) + "/" + 
                          std::to_string(tspSteps.size()) + ": " + step.description;
        RenderUtils::drawText(-0.95f, 0.95f, info.c_str());
        
        if (step.isFinalTour) {
            double length = TSPAlgorithm::calculateTourLength(cities, step.assignment);
            std::string lengthStr = "Tour Length: " + std::to_string(static_cast<int>(length));
            RenderUtils::drawText(-0.95f, 0.88f, lengthStr.c_str());
            
            if (salesmanAnimating) {
                glColor3f(0.0f, 1.0f, 0.0f);
                std::string speedInfo = "Salesman traveling (" + std::string(fastMode ? "FAST" : "SLOW") + ") | A: Stop | F: Toggle Speed";
                RenderUtils::drawText(-0.95f, 0.81f, speedInfo.c_str());
            } else {
                glColor3f(0.7f, 0.7f, 0.7f);
                RenderUtils::drawText(-0.95f, 0.81f, "Press A to animate salesman");
            }
        }
    }
    
    // Draw matrix panel if enabled
    if (showMatrix && currentStepIndex >= 0 && currentStepIndex < tspSteps.size()) {
        MatrixPanel::draw(cities, tspSteps[currentStepIndex]);
    }
    
    // Instructions
    glColor3f(0.7f, 0.7f, 0.7f);
    RenderUtils::drawText(-0.95f, -0.95f, "N: Next | P: Previous | S: Solve | M: Matrix | A: Animate | F: Fast | Click: Add");
    
    glutSwapBuffers();
}

// Keyboard handler
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'n':  // Next step
        case 'N':
            if (currentStepIndex < static_cast<int>(tspSteps.size()) - 1) {
                currentStepIndex++;
                std::cout << "Step " << currentStepIndex + 1 << "/" << tspSteps.size() << std::endl;
                salesmanAnimating = false;
                glutPostRedisplay();
            }
            break;
            
        case 'p':  // Previous step
        case 'P':
            if (currentStepIndex > 0) {
                currentStepIndex--;
                std::cout << "Step " << currentStepIndex + 1 << "/" << tspSteps.size() << std::endl;
                salesmanAnimating = false;
                glutPostRedisplay();
            }
            break;
            
        case 's':  // Solve
        case 'S':
            solveTSP();
            break;
            
        case 'm':  // Toggle matrix
        case 'M':
            showMatrix = !showMatrix;
            glutPostRedisplay();
            break;
            
        case 'a':  // Animate salesman
        case 'A':
            if (currentStepIndex >= 0 && currentStepIndex < tspSteps.size()) {
                if (tspSteps[currentStepIndex].isFinalTour) {
                    salesmanAnimating = !salesmanAnimating;
                    if (salesmanAnimating) {
                        salesmanProgress = 0.0f;
                        salesmanCurrentEdge = 0;
                        glutTimerFunc(16, timer, 0);
                        std::cout << "Salesman animation started!" << std::endl;
                    } else {
                        std::cout << "Salesman animation stopped" << std::endl;
                    }
                    glutPostRedisplay();
                }
            }
            break;
            
        case 'f':  // Toggle fast mode
        case 'F':
            if (salesmanAnimating) {
                fastMode = !fastMode;
                animationSpeed = fastMode ? 0.03f : 0.005f;
                std::cout << "Animation speed: " << (fastMode ? "FAST" : "SLOW") << std::endl;
                glutPostRedisplay();
            }
            break;
            
        case 'q':  // Quit
        case 'Q':
        case 27:   // ESC
            exit(0);
            break;
    }
}

// Mouse handler - add cities by clicking
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Store pixel coordinates directly
        City newCity;
        newCity.name = "City" + std::to_string(cities.size());
        newCity.x = x;
        newCity.y = y;
        newCity.orig_x = x;
        newCity.orig_y = y;
        
        cities.push_back(newCity);
        std::cout << "Added " << newCity.name << " at (" << newCity.orig_x 
                  << ", " << newCity.orig_y << ")" << std::endl;
        
        // Reset solution
        tspSteps.clear();
        currentStepIndex = -1;
        salesmanAnimating = false;
        
        glutPostRedisplay();
    }
}

// Reshape handler
void reshape(int w, int h) {
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("TSP Solver - Hungarian Algorithm + Subtour Patching");
    glutFullScreen();
    
    // Set up OpenGL
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
    
    // Load initial cities
    loadCitiesFromJSON("cities.json");
    
    std::cout << "\n=== Interactive TSP Solver ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  S - Solve TSP using Hungarian Algorithm" << std::endl;
    std::cout << "  N - Next step in animation" << std::endl;
    std::cout << "  P - Previous step" << std::endl;
    std::cout << "  M - Toggle matrix display" << std::endl;
    std::cout << "  A - Animate traveling salesman (on final tour)" << std::endl;
    std::cout << "  Click - Add new city" << std::endl;
    std::cout << "  Q/ESC - Quit" << std::endl;
    
    glutMainLoop();
    return 0;
}
