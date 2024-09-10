#include "GLGraphics.hpp"
#include "Context.hpp"

int main()
{
    Context context{};
    GLGraphics graphics{};

    while (context.window_open()) {
        context.update();
    }
}
