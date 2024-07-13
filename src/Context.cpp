#include "Context.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

namespace
{
constexpr int WIDTH = 500;
constexpr int HEIGHT = 500;
constexpr char TITLE[] = "window";

static void glfw_error(int id, const char* description)
{
    std::cout << description << "\n";
}
}

Context::Context()
{
    glfwSetErrorCallback(&glfw_error);

    if (!glfwInit()) {
        std::cout << "failed to initialize GLFW\n";
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT,
                              TITLE,
                              NULL,
                              NULL);
    if (window == nullptr) {
        std::cout << "failed to create window\n";
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    glfwSwapBuffers(window);
}

Context::~Context()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Context::window_open()
{
    return !glfwWindowShouldClose(window);
}

void Context::update()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}
