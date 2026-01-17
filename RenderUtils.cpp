#include "RenderUtils.hpp"
#include <GL/freeglut.h>
#include <string>

namespace RenderUtils {
    void drawText(float x, float y, const std::string& text) {
        glRasterPos2f(x, y);
        for (char c : text) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }
    
    void drawText(float x, float y, const char* text) {
        glRasterPos2f(x, y);
        while (*text) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *text);
            text++;
        }
    }
}
