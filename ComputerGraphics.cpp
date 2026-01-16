#include <GL/freeglut.h>
#include "City.hpp"
#include "TSPAlgorithm.hpp"
#include "MatrixPanel.hpp"
#include "RenderUtils.hpp"
#include <vector>
#include <string>
#include <iostream>

const int WIDTH = 1280, HEIGHT = 800;
std::vector<City> cities;
std::vector<std::vector<float>> distMatrix;
std::vector<Step> steps;
int step_index = -1;
bool prepared = false;

void toGlCoords(int winX, int winY, float& glX, float& glY) {
    glX = 2.0f * (float)winX / WIDTH - 1.0f;
    glY = 1.0f - 2.0f * (float)winY / HEIGHT;
}

void loadCitiesFromJsonFileModular(const std::string& filename) {
    loadCitiesFromJsonFile(filename, cities);
}

void computeDistanceMatrixModular() {
    computeDistanceMatrix(cities, distMatrix);
}

void prepareStepsModular() {
    prepareSteps(cities, distMatrix, steps);
    step_index = 0;
    prepared = true;
}

void renderMatrixPanel() {
    renderMatrixAndAssignment(distMatrix, steps, step_index);
}

void renderCurrentPath() {
    if(step_index < 0 || step_index >= int(steps.size())) return;
    const auto& st = steps[step_index];
    size_t N = st.assignment.size();
    if(N == 0) return;
    glColor3f(0.2f,1,0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    int cur = 0, cnt=0;
    std::vector<bool> seen(N, false);
    for (cur=0; cnt<N; ++cnt) {
        seen[cur]=true;
        glVertex2f(cities[cur].x, cities[cur].y);
        if(st.assignment[cur]==-1) break;
        cur = st.assignment[cur];
        if(cur<0||cur>=int(N)||seen[cur]) break;
    }
    glEnd();
}

void display() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // --- LEFT ---
    glViewport(0, 0, 0.7*WIDTH, HEIGHT);
    glPointSize(14.0f);
    glColor3f(1, 0.8f, 0.1f);
    glBegin(GL_POINTS);
    for (auto& c : cities) glVertex2f(c.x, c.y);
    glEnd();
    renderCurrentPath();
    // --- RIGHT ---
    glViewport(0.7*WIDTH, 0, 0.3*WIDTH, HEIGHT);
    renderMatrixPanel();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float gx, gy; toGlCoords(x, y, gx, gy);
        cities.push_back({gx, gy, gx, gy});
        computeDistanceMatrixModular();
        prepared = false;
        steps.clear(); step_index = -1;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == 'c' || key == 'C') { cities.clear(); steps.clear(); step_index = -1; prepared=false; glutPostRedisplay(); }
    if (key == 'r' || key == 'R') { computeDistanceMatrixModular(); steps.clear(); step_index = -1; prepared=false; glutPostRedisplay(); }
}

void specialkeys(int key, int x, int y) {
    if (cities.size() < 2) return;
    if (!prepared) { computeDistanceMatrixModular(); prepareStepsModular(); }
    if (key == GLUT_KEY_RIGHT) {
        if (step_index+1 < int(steps.size())) step_index++;
        glutPostRedisplay();
    } else if (key == GLUT_KEY_LEFT) {
        if (step_index-1 >= 0) step_index--;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("TSP Visualizer - Step-Through Assignment Demo (GLUT)");
    if (argc > 1) loadCitiesFromJsonFileModular(argv[1]);
    if (!cities.empty()) computeDistanceMatrixModular();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialkeys);
    glutMainLoop();
    return 0;
}
