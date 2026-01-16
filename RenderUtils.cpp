#include "RenderUtils.hpp"
#include <GL/freeglut.h>
#include <string>
void drawText(float x, float y, const char* str, float r, float g, float b) {
    glColor3f(r,g,b);
    glRasterPos2f(x, y);
    for(const char* p=str; *p; ++p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
}
