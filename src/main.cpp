#include "GLGraphics.hpp"
#include "Context.hpp"
#include "Polynomial.hpp"
#include <format>

int main()
{
    Variable x;
    std::cout << x << "\n";

    std::cout << (x^7) << "\n";

    Variable y;
    Variable z;
    PowerPermutation pp = (x^2)*(y^1)*(z^3);
    // {
    //     {x, 2},
    //     {y, 1},
    //     {z, 3}
    // };

    std::cout << pp << "\n";

    Polynomial<double> poly = pp * 3.71 + (x^1)*(y^3) * 2.0;

    std::cout << "poly: " << poly << "\n";

    double result = poly(
    {
        {x, 2.0},
        {y, 3.0},
        {z, 0.5}
    });

    std::cout << std::format("result is: {}\n", result);


    // Context context{};
    // GLGraphics graphics{};
    //
    // while (context.window_open()) {
    //     context.update();
    // }
}
