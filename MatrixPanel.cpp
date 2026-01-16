#include "MatrixPanel.hpp"
#include "RenderUtils.hpp"
#include <vector>
#include <algorithm>
void renderMatrixAndAssignment(const std::vector<std::vector<float>>& distMatrix, const std::vector<Step>& steps, int step_index) {
    size_t N = distMatrix.size();
    if (N==0 || step_index<0 || step_index>=int(steps.size())) return;
    constexpr float vpad = 0.15f;
    float panelYmin = -0.95f, panelYmax = +0.95f;
    float panelXmin = -0.95f, panelXmax = +0.95f;
    float yTopInfo    = panelYmax;
    float yTopMatrix  = panelYmax - vpad*1.9f;
    float yBotMatrix  = panelYmin + vpad*1.9f;
    float yBotInfo    = panelYmin;
    float matrixHeight = yTopMatrix - yBotMatrix;
    float matrixWidth  = panelXmax - panelXmin;
    float gridSize = std::min(matrixHeight, matrixWidth);
    float squareX0 = panelXmin + 0.5f*(matrixWidth  - gridSize);
    float squareX1 = squareX0 + gridSize;
    float squareY0 = yBotMatrix;
    float squareY1 = yBotMatrix + gridSize;
    float cellW = (squareX1-squareX0)/(N+1);
    float cellH = (squareY1-squareY0)/(N+1);
    char buf[32];
    // Top info bar
    char stepTop[128], stepBot[128];
    if (step_index == 0) {
        snprintf(stepTop, 120, "Step %d of %d: Start", step_index, (int)steps.size()-1);
        snprintf(stepBot, 120, "Press \342\206\222 to advance step");
    } else if (step_index == (int)steps.size() - 1) {
        snprintf(stepTop, 120, "Step %d (Final): All assignments completed", step_index);
        snprintf(stepBot, 120, "Press \342\206\220 to go back; Assignment complete.");
    } else {
        int k = steps[step_index].current_step;
        int row = steps[step_index].working_row;
        int col = steps[step_index].working_col;
        if(row>=0 && col>=0)
            snprintf(stepTop, 120, "Step %d of %d: Assigning row %d \342\206\222 col %d", step_index, (int)steps.size()-1, row+1, col+1);
        else
            snprintf(stepTop, 120, "Step %d of %d", step_index, (int)steps.size()-1);
        const auto& prev = steps[step_index-1];
        int prow = prev.working_row, pcol = prev.working_col;
        if(prow>=0 && pcol>=0)
            snprintf(stepBot, 120, "Prev: Row %d assigned to Col %d. Press \342\206\220 to step back, \342\206\222 to advance.", prow+1, pcol+1);
        else
            snprintf(stepBot, 120, "Press \342\206\220/\342\206\222 to step");
    }
    drawText(squareX0, yTopInfo-0.03f, stepTop, 0.7f,1,1);
    // Axis labels
    for(size_t i=0; i<N; ++i) {
        snprintf(buf, 10, "%zu", i+1);
        drawText(squareX0+(i+1)*cellW, squareY1+cellH*0.2f, buf,1,1,0);
        drawText(squareX0-cellW*0.9f, squareY1-(i+1)*cellH, buf,0,1,1);
    }
    // Matrix cell values
    const auto& st = steps[step_index];
    for(size_t i=0; i<N; ++i) {
        for(size_t j=0; j<N; ++j) {
            snprintf(buf, 16, "%4.0f", distMatrix[i][j]);
            float r=0.8,g=0.8,b=0.8;
            if(st.working_row == int(i) && st.working_col == int(j)) { r = 1; g = 0.5; b = 0.2; }
            else if(st.working_row == int(i) || st.working_col == int(j)) { r = 1; g = 1; b = 0.6; }
            if(st.assignment[i] == int(j) && st.assignment[i]!=-1) { r=0.2; g=1; b=0.2; }
            drawText(squareX0+(j+1)*cellW, squareY1-(i+1)*cellH, buf, r, g, b);
        }
    }
    drawText(squareX0, yBotInfo+0.035f, stepBot, 1,1,1);
}
