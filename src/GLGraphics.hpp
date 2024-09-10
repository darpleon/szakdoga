#pragma once

#include "Shader.hpp"
#include <optional>

class GLGraphics
{
public:
    GLGraphics();

private:
    std::optional<Shader> shader;
};
