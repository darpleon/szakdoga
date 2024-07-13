#include <GLFW/glfw3.h>

class Context {
public:
    Context();
    ~Context();

    bool window_open();

    void update();

private:
    GLFWwindow* window;
};
