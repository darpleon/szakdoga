#include <cmath>
#include <cstddef>
#include <cstdio>
#include <format>
#include <iostream>
#include <array>
#include <eigen3/Eigen/Core>
#include <iterator>
#include <ostream>
#include <print>

/*using v<3> = Eigen::Vector3d;*/
/*using v<4> = Eigen::Vector4d;*/
/*using rv2 = Eigen::RowVector2d;*/
/*using rv<3> = Eigen::RowVector3d;*/

template <int N>
using V = Eigen::Matrix<double, N, 1>;

template <int N>
using RV = Eigen::Matrix<double, 1, N>;

template <int R, int C>
using M = Eigen::Matrix<double, R, C>;


template<typename T>
class grid
{
public:
    grid(size_t n, size_t m)
        : _n{n}, _m{m}, _d(n * m)
    {
    }

    grid(grid&& g) : _n{g._n}, _m{g._m}, _d{std::move(g._d)} {}

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

struct idx2d
{
    idx2d(size_t m, size_t i, size_t j) :m(m), idx{i, j} {}

    std::array<size_t, 2> operator*()
    {
        return idx;
    }

    void operator++()
    {
        if (idx[1] == m - 1) {
            ++idx[0];
            idx[1] = 0;
        }
        else {
            ++idx[1];
        }
    }

    bool operator!=(const idx2d& rhs) const
    {
        return idx != rhs.idx;
    }
private:
    std::array<size_t, 2> idx;
    size_t m;
};

class range2d
{
public:
    range2d(size_t n, size_t m) :n(n), m(m) {}

    idx2d begin()
    {
        return idx2d {m, 0, 0};
    }

    idx2d end()
    {
        return idx2d {m, n, 0};
    }

private:
    size_t n, m;
};


template <typename T>
range2d indices(const grid<T>& g)
{
    return range2d(g.n(), g.m());
}


double h(V<3> p, V<3> n)
{
    return p.dot(n);
}

V<3> iota(V<3> n, double h)
{
    return (1. / (1. - n.z())) * V<3>{n.x(), n.y(), h};
}

V<4> iota_inv(V<3> a)
{
    return 1. / (1. + a.x() * a.x() + a.y() * a.y()) * V<4>{2 * a.x(),
                                                          2 * a.y(),
                                                          1. - a.x() * a.x() - a.y() * a.y(),
                                                          2 * a.z()};
}

M<4, 3> iota_inv_jacobian(V<3> a)
{
    const double& x = a.x();
    const double& y = a.y();
    const double& z = a.z();
    double x2 = a.x() * a.x();
    double y2 = a.y() * a.y();
    double one_plus = (1. + x2 + y2);
    return 2. / (one_plus * one_plus) * M<4, 3>{ {1. - x2 + y2,  -2. * x * y, 0.},
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

template <typename T>
std::function<std::vector<T>(std::function<T(size_t)>)> value_generator(unsigned int res)
{
    return [res] (std::function<T(size_t)> f) {
        std::vector<T> values{};
        values.reserve(res + 1);

        for (size_t i = 0; i <= res; ++i) {
            values.push_back(f(i));
        }
        return values;
    };
}

V<4> ferguson_vec(double t)
{
    return V<4>{F0(t), F1(t), F2(t), F3(t)};
}

std::function<V<4>(size_t)> ferguson_func(const std::vector<double> t)
{
    return [&t] (size_t i) -> V<4> {
        return ferguson_vec(t[i]);
    };
}

std::function<V<3>(size_t)> boundary(M<3, 4> interp, std::vector<V<4>> ferguson)
{
    return [&interp, &ferguson] (size_t i) -> V<3> {
        return interp * ferguson[i];
    };
}

std::function<double(size_t)> t_func(unsigned int res)
{
    return [res] (size_t i) -> double {
        return static_cast<double>(i) / static_cast<double>(res);
    };
}

grid<double> calculate_coons(size_t res, grid<M<3, 4>> interp)
{
    grid<double> y{res + 1, res + 1};
    auto generate_double = value_generator<double>(res);
    /*auto generate_v<4> = value_generator<v<4>>(res);*/
    /*auto generate_v<3> = value_generator<v<3>>(res);*/
    /*std::vector<double> t = generate_double(t_func(res));*/
    /*std::vector<v<4>> F = generate_v<4>(ferguson_func(t));*/
    
    return y;
}

std::function<double(size_t)> interp_func(const std::vector<double>& t)
{
    return [t] (size_t i) {
        return t[i] * 2.;
    };
}

V<3> calculate_gamma_star(const grid<V<3>>& a, size_t i, size_t j)
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

V<3> calculate_delta_star(const grid<V<3>>& a, size_t i, size_t j)
{
    if (j == 0) {
        return 2. * (a[i, 1] - a[i, 0]);
    }
    else if (j == a.n() - 1) {
        return 2. * (a[i, a.n() - 1] - a[i, a.n() - 2]);
    }
    else {
        return a[i, j + 1] - a[i, j - 1];
    }
}

template <int S, int R, int C>
M<S * R, S * C> inflate(M<R, C> m)
{
    M<S * R, S * C> f;
    for (auto [i, j] : range2d{R, C}) {
        f.template block<S, S>(S * i, S * j) = m(i, j) * M<S, S>::Identity();
    }
    return f;
}


grid<M<6, 2>> calculate_interp(const grid<V<3>>& p, const grid<V<3>>& n)
{
    grid<double> h{3, 3};
    grid<V<3>> a{3, 3};
    grid<M<3, 3>> dn_dy{3, 3};
    grid<RV<3>> dh_dy{3, 3};
    grid<V<3>> b{3, 3};

    for (auto [i, j] : range2d{3, 3}) {
        h[i, j] = p[i, j].dot(n[i, j]);
        a[i, j] = iota(n[i, j], h[i, j]);
        M<4, 3> jac = iota_inv_jacobian(a[i, j]);
        dn_dy[i, j] = jac.topRows<3>();
        dh_dy[i, j] = jac.bottomRows<1>();
        b[i, j] = (p[i, j].transpose() * dn_dy[i, j] - dh_dy[i, j]).transpose();
    }

    /*grid<V<3>> gamma{3, 3};*/
    /*grid<V<3>> delta{3, 3};*/

    grid<M<6, 2>> interp{3, 3};

    for (auto [i, j] : range2d{3, 3}) {
        V<3> gamma_star = calculate_gamma_star(a, i, j);
        V<3> gamma = gamma_star - b[i, j].dot(gamma_star) * b[i, j];

        V<3> delta_star = calculate_delta_star(a, i, j);
        V<3> delta = delta_star - b[i, j].dot(delta_star) * b[i, j];

        interp[i, j] << a[i, j], delta, gamma, V<3>::Zero();
    }

    /*grid<M<12, 4>> interp{2, 2};*/
    /**/
    /*for (auto [i, j] : range2d{2, 2})*/
    /*{*/
    /*    auto block = [i, j] (const grid<V<3>>& g) {*/
    /*        M<6, 2> block;*/
    /*        block << g[i, j], g[i, j + 1], g[i + 1, j], g[i + 1, j + 1];*/
    /*        return block;*/
    /*    };*/
    /*    interp[i, j] << block(a), block(delta), block(gamma), M<6, 2>::Zero();*/
    /*}*/

    return interp;
}

int main()
{
    grid<V<3>> p{3, 3, { V<3>{0., 0., 0.},          V<3>{0., -11./72., -1./12.},    V<3>{0., -2./9., -1./3.},
                         V<3>{11./72., 0., 1./12.}, V<3>{7./36., -7./36., 0.},      V<3>{23./72., -11./36., -1./4.},
                         V<3>{2./9., 0., 1./3.},    V<3>{11./36., -23./72., 1./4.}, V<3>{5./9., -5./9., 0.} }};

    grid<V<3>> n{3, 3, { V<3>{0., 0., -1.},       V<3>{0., 4./5., -3./5.},    V<3>{0., 1., 0.},
                         V<3>{4./5., 0., -3./5.}, V<3>{2./3., 2./3., -1./3.}, V<3>{4./9., 8./9., 1./9.},
                         V<3>{1., 0., 0.},        V<3>{8./9., 4./9., 1./9.},  V<3>{2./3., 2./3., 1./3.} }};

    grid<M<6, 2>> interp = calculate_interp(p, n);
    

    for (auto [i, j] : indices(interp)) {
        std::println("i = {}, j = {}", i, j);
        std::cout << interp[i, j] << "\n\n";
    }

}
