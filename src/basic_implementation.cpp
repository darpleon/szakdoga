#include <format>
#include <iostream>
#include <array>
#include <eigen3/Eigen/Core>
#include <functional>

using v3 = Eigen::Vector3d;
using v4 = Eigen::Vector4d;
using m34 = Eigen::Matrix<double, 4, 3>;

template<typename T, int N, int M = N>
using grid = std::array<std::array<T, M>, N>;

template<typename T, int N, int M>
void foreach_grid(grid<T, N, M> g, std::function<void(int, int)> f)
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            f(i, j);
        }
    }
}

template<int N, int M>
constexpr grid<int, N * M, 2> indices_2d_gen()
{
    grid<int, N * M, 2> idx{};
    int c = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            idx[c] = {i, j};
            ++c;
        }
    }
    return idx;
}

template<int N, int M>
constexpr grid<int, N * M, 2> indices_2d = indices_2d_gen<N, M>();

double h(v3 p, v3 n)
{
    return p.dot(n);
}

v3 iota(v3 n, double h)
{
    return 1. / (1. - n.z()) * v3{n.x(), n.y(), h};
}

v4 iota_inv(v3 a)
{
    return 1. / (1. + a.x() * a.x() + a.y() * a.y()) * v4{2 * a.x(),
                                                          2 * a.y(),
                                                          1. - a.x() * a.x() - a.y() * a.y(),
                                                          2 * a.z()};
}

m34 iota_inv_jacobian(v3 a)
{
    const double& x = a.x();
    const double& y = a.y();
    const double& z = a.z();
    double x2 = a.x() * a.x();
    double y2 = a.y() * a.y();
    double one_plus = (1. + x2 + y2);
    return 1. / (one_plus * one_plus) * m34{ {1. - x2 + y2,  -2. * x * y, 0.},
                                             { -2. * x * y, 1. + x2 - y2, 0.},
                                             {     -2. * x,      -2. * y, 0.},
                                             { -2. * x * z,  -2. * y * z, 1.} };
}

template<int N, int M>
grid<v3, N, M> calculate_gamma_star(grid<v3, N, M> a)
{

}


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

    std::cout << v3{1, 2, 3}[2] << "\n";

    m34 test {{1,  2,  3},
              {4,  5,  6},
              {7,  8,  9},
             {10, 11, 12}};

    std::cout << test << "\n";
    for (auto [i, j] : indices_2d<3, 4>) {
        std::cout << std::format("({}, {})\n", i, j);
    }
}
