#include <cstddef>
#include <format>
#include <iostream>
#include <array>
#include <eigen3/Eigen/Core>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>
#include <numeric>

using v3 = Eigen::Vector3d;
using v4 = Eigen::Vector4d;
using m34 = Eigen::Matrix<double, 4, 3>;

/*template<typename T, int N, int M = N>*/
/*using grid = std::array<std::array<T, M>, N>;*/
/**/
/*template<typename T, int N, int M>*/
/*void foreach_grid(grid<T, N, M> g, std::function<void(int, int)> f)*/
/*{*/
/*    for (int i = 0; i < N; ++i) {*/
/*        for (int j = 0; j < M; ++j) {*/
/*            f(i, j);*/
/*        }*/
/*    }*/
/*}*/
/**/
/*template<int N, int M>*/
/*constexpr grid<int, N * M, 2> indices_2d_gen()*/
/*{*/
/*    grid<int, N * M, 2> idx{};*/
/*    int c = 0;*/
/*    for (int i = 0; i < N; ++i) {*/
/*        for (int j = 0; j < M; ++j) {*/
/*            idx[c] = {i, j};*/
/*            ++c;*/
/*        }*/
/*    }*/
/*    return idx;*/
/*}*/
/**/
/*template<int N, int M>*/
/*constexpr grid<int, N * M, 2> indices_2d = indices_2d_gen<N, M>();*/

template<typename T>
class grid
{
public:
    grid(size_t n, size_t m)
        : n(n), m(m), d(n * m)
    {
    }

    grid(size_t n, size_t m, std::initializer_list<T> contents)
        : n(n), m(m), d(n * m)
    {
        std::ranges::copy(contents.begin(), contents.end(), d.begin());
    }

    const T& operator[](size_t i, size_t j) const
    {
        return d[i * n + j];
    }

    T& operator[](size_t i, size_t j)
    {
        return d[i * n + j];
    }

private:
    const size_t n, m;
    std::vector<T> d;

};

class idx2d
{
public:
    idx2d(size_t n, size_t m) :n(n), m(m) {}

    struct iterator
    {
        iterator(size_t m, size_t i, size_t j) :m(m), i(i), j(j) {}

        std::array<size_t, 2> operator*()
        {
            return {i, j};
        }

        void operator++()
        {
            if (j == m - 1) {
                ++i;
                j = 0;
            }
            else {
                ++j;
            }
        }

        bool operator!=(const iterator& rhs) const
        {
            return i != rhs.i || j != rhs.j;
        }
    private:
        size_t m, i, j;
    };


    iterator begin()
    {
        return iterator {m, 0, 0};
    }

    iterator end()
    {
        return iterator {m, n, 0};
    }

private:
    size_t n, m;
};


/*std::function<std::array<size_t, 2>(size_t)> idx_splitter(size_t n, size_t m)*/
/*{*/
/*    return [n, m] (size_t idx) {*/
/*        return std::array<size_t, 2>{idx / n, idx & m};*/
/*    };*/
/*}*/

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



int main()
{
    grid<int> g(2, 2, {1, 2, 3, 4});
    for (auto [i, j] : idx2d(2, 2)) {
        std::cout << g[i, j] << "\n";
    }

    std::array<int, 4> a{1, 2, 3, 4};
    auto s = std::ranges::subrange(a.begin() + 1, a.begin() + 3);
    s[1] = 9;
    std::cout << a[2] << "\n";
}
