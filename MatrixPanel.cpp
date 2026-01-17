#include "MatrixPanel.hpp"
#include "RenderUtils.hpp"
#include <GL/freeglut.h>
#include <sstream>
#include <iomanip>
#include <cmath>

void MatrixPanel::draw(const std::vector<City>& cities, const TSPStep& step) {
    int n = cities.size();
    if (n == 0) return;
    
    // Panel background
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float panelWidth = 0.5f;
    float panelHeight = 0.8f;
    float panelX = 0.45f;
    float panelY = 0.35f;
    
    glBegin(GL_QUADS);
    glVertex2f(panelX, panelY);
    glVertex2f(panelX + panelWidth, panelY);
    glVertex2f(panelX + panelWidth, panelY + panelHeight);
    glVertex2f(panelX, panelY + panelHeight);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // Title
    glColor3f(1.0f, 1.0f, 0.0f);
    RenderUtils::drawText(panelX + 0.02f, panelY + panelHeight - 0.05f, 
                         ("Assignment Matrix - Iteration " + std::to_string(step.iteration)).c_str());
    
    // Subtour info
    float yPos = panelY + panelHeight - 0.12f;
    glColor3f(0.8f, 0.8f, 1.0f);
    
    std::string subtourInfo = "Subtours detected: " + std::to_string(step.subtours.size());
    RenderUtils::drawText(panelX + 0.02f, yPos, subtourInfo.c_str());
    yPos -= 0.06f;
    
    // List each subtour
    for (size_t i = 0; i < step.subtours.size(); i++) {
        std::stringstream ss;
        ss << "  Subtour " << i << " (size " << step.subtours[i].size() << "): ";
        
        for (size_t j = 0; j < step.subtours[i].size() && j < 8; j++) {
            ss << step.subtours[i][j];
            if (j < step.subtours[i].size() - 1) ss << "->";
        }
        if (step.subtours[i].size() > 8) ss << "...";
        
        const float colors[][3] = {
            {0.0f, 1.0f, 0.0f},  // Green
            {1.0f, 0.0f, 0.0f},  // Red
            {1.0f, 1.0f, 0.0f},  // Yellow
            {1.0f, 0.0f, 1.0f},  // Magenta
            {0.0f, 1.0f, 1.0f},  // Cyan
        };
        glColor3fv(colors[i % 5]);
        
        RenderUtils::drawText(panelX + 0.04f, yPos, ss.str().c_str());
        yPos -= 0.05f;
    }
    
    yPos -= 0.03f;
    
    // Assignment edges
    glColor3f(1.0f, 1.0f, 1.0f);
    RenderUtils::drawText(panelX + 0.02f, yPos, "Assignment Edges:");
    yPos -= 0.05f;
    
    // Display assignment in compact form with wrapping
    int edgeCount = 0;
    std::stringstream lineSS;
    int itemsInLine = 0;
    const int maxItemsPerLine = 3;  // Show 3 edges per line
    
    for (const auto& [from, to] : step.assignment) {
        if (edgeCount >= 12) {  // Show up to 12 edges (4 lines)
            if (itemsInLine > 0) {
                glColor3f(0.7f, 0.7f, 0.7f);
                RenderUtils::drawText(panelX + 0.04f, yPos, lineSS.str().c_str());
                yPos -= 0.04f;
            }
            RenderUtils::drawText(panelX + 0.04f, yPos, "...");
            yPos -= 0.04f;
            break;
        }
        
        // Calculate distance
        double dx = cities[from].orig_x - cities[to].orig_x;
        double dy = cities[from].orig_y - cities[to].orig_y;
        double dist = std::sqrt(dx * dx + dy * dy);
        
        // Add to current line
        if (itemsInLine > 0) {
            lineSS << " | ";
        }
        lineSS << from << "->" << to << "(" << std::fixed << std::setprecision(0) << dist << ")";
        itemsInLine++;
        
        // Wrap to next line if needed
        if (itemsInLine >= maxItemsPerLine) {
            glColor3f(0.7f, 0.7f, 0.7f);
            RenderUtils::drawText(panelX + 0.04f, yPos, lineSS.str().c_str());
            yPos -= 0.04f;
            lineSS.str("");
            lineSS.clear();
            itemsInLine = 0;
        }
        
        edgeCount++;
    }
    
    // Print remaining items if any
    if (itemsInLine > 0) {
        glColor3f(0.7f, 0.7f, 0.7f);
        RenderUtils::drawText(panelX + 0.04f, yPos, lineSS.str().c_str());
        yPos -= 0.04f;
    }
    
    // Status
    yPos -= 0.03f;
    if (step.isFinalTour) {
        glColor3f(0.0f, 1.0f, 0.0f);
        RenderUtils::drawText(panelX + 0.02f, yPos, "STATUS: COMPLETE TOUR FOUND!");
    } else {
        glColor3f(1.0f, 0.5f, 0.0f);
        RenderUtils::drawText(panelX + 0.02f, yPos, "STATUS: Patching subtours...");
    }
}
