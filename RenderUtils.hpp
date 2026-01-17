#ifndef RENDERUTILS_HPP
#define RENDERUTILS_HPP

#include <string>

namespace RenderUtils {
    void drawText(float x, float y, const std::string& text);
    void drawText(float x, float y, const char* text);
}

#endif
