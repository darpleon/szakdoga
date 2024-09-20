#include "GLGraphics.hpp"
#include "Context.hpp"
#include "Polynomial.hpp"
#include <format>

int main()
{
    Variable x;
    std::cout << x << "\n";

    std::cout << (x^7) << "\n";

    std::cout << std::format("size of uint:{}\n", sizeof(unsigned int));

    Variable y;
    Variable z;
    PowerPermutation pp
    {
        {x, 2},
        {y, 1},
        {z, 3}
    };

    std::cout << pp << "\n";

    Polynomial<double> poly{pp};


    // Context context{};
    // GLGraphics graphics{};
    //
    // while (context.window_open()) {
    //     context.update();
    // }
}
