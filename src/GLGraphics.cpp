#include "GLGraphics.hpp"
#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const char* message, const void* userParam) {
    std::cout << message << "\n";
}

GLGraphics::GLGraphics()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_callback, NULL);

    shader = Shader("src/shader/basic.vs", "src/shader/basic.fs");
    shader->use();
}
