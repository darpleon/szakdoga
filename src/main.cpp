#include "Context.hpp"

int main()
{
    Context context;

    while (context.window_open()) {
        context.update();
    }
}
