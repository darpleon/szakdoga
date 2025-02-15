#include <cmath>
#include <cstddef>
#include <cstdio>
#include <format>
#include <iostream>
#include <array>
#include <eigen3/Eigen/Core>
#include <functional>
#include <iterator>
#include <ostream>
#include <ranges>
#include <utility>
#include <numeric>
#include <print>

using v3 = Eigen::Vector3d;
using v4 = Eigen::Vector4d;
using rv2 = Eigen::RowVector2d;
using rv3 = Eigen::RowVector3d;
using m34 = Eigen::Matrix<double, 4, 3>;


template<typename T>
class grid
{
public:
    grid(size_t n, size_t m)
        : _n(n), _m(m), _d(n * m)
    {
    }

    size_t n() const
    {
        return _n;
    }

    size_t m() const
    {
        return _m;
    }

    grid(size_t n, size_t m, std::initializer_list<T> contents)
        : _n(n), _m(m), _d(n * m)
    {
        std::ranges::copy(contents.begin(), contents.end(), _d.begin());
    }

    const T& operator[](size_t i, size_t j) const
    {
        return _d[i * _n + j];
    }

    T& operator[](size_t i, size_t j)
    {
        return _d[i * _n + j];
    }

private:
    const size_t _n, _m;
    std::vector<T> _d;

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

template <typename T>
idx2d indices(grid<T> g)
{
    return idx2d(g.n(), g.m());
}


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

double F0(double t)
{
    return 2. * t * t * t  - 3. * t * t + 1.;
}

double F0d(double t)
{
    return 6. * t * t - 6. * t;
}

double F1(double t)
{
    return 1. - F0(t);
}

double F1d(double t)
{
    return -F0d(t);
}

double F2(double t)
{
    return t * t * t - 2. * t * t + t;
}

double F2d(double t)
{
    return 3. * t * t - 4. * t + 1.;
}

double F3(double t)
{
    return t * t * t - t * t;
}

double F3d(double t)
{
    return 3. * t * t - 2. * t;
}

grid<v3> to_isotropic(const grid<v3>& n, const grid<double>& h)
{
    grid<v3> a{n.n(), n.m()};
    for (auto [i, j] : indices(a)) {
        a[i, j] = iota(n[i, j], h[i, j]);
    }
    return a;
}

v3 calculate_gamma_star(const grid<v3>& a, size_t i, size_t j)
{
    if (i == 0) {
        return 2. * (a[1, j] - a[0, j]);
    }
    else if (i == a.n() - 1) {
        return 2. * (a[a.n() - 1, j] - a[a.n() - 2, j]);
    }
    else {
        return a[i + 1, j] - a[i - 1, j];
    }
}

v3 project(const v3& v, const v3& n, double d = 0.)
{
    return v - (v.dot(n) - d) * n;
}

grid<v3> calculate_gamma_star(const grid<v3>& a)
{
    grid<v3> gs{a.n(), a.m()};
    for (auto [i, j] : indices(gs)) {
        gs[i, j] = calculate_gamma_star(a, i, j);
    }
    return gs;
}

/*v3 calculate_b(const v3& p, const v3)*/


int main()
{
    grid<v3> pij{2, 2};
    grid<v3> nij{2, 2};

    for (auto [i, j] : idx2d(2, 2)) {

    }
}
