#include <iostream>
#include <array>

template<typename T, int N, int M = N>
using grid = std::array<std::array<T, M>, N>;

int main()
{
    grid<int, 2> g = {{ {1, 2},
                        {3, 4} }};

    for (auto& line: g) {
        bool first = true;
        for (int n: line) {
            if (first) {
                first = false;
            }
            else {
                std::cout << ", ";
            }
            std::cout << n;
        }
        std::cout << "\n";
    }
}
