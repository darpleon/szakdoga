#include <cmath>
#include <cstddef>
#include <cstdio>
#include <array>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>
#include <print>

#include "grid.hpp"
#include "io.hpp"
#include "matrix_utility.hpp"


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
                                                          -(1. - a.x() * a.x() - a.y() * a.y()),
                                                          2 * a.z()};
}

M<4, 3> jacobian_numerator(V<3> a)
{
    const double& x = a.x();
    const double& y = a.y();
    const double& z = a.z();
    double x2 = a.x() * a.x();
    double y2 = a.y() * a.y();
    return M<4, 3>{ {1. - x2 + y2,  -2. * x * y, 0.},
                    { -2. * x * y, 1. + x2 - y2, 0.},
                    {     2. * x,      2. * y, 0.},
                    { -2. * x * z,  -2. * y * z, 1.} };
}

double jacobian_q(V<3> a)
{
    return (1. + a[0] * a[0] + a[1] * a[1]);
}

M<4, 3> iota_inv_jacobian(V<3> a)
{
    double q = jacobian_q(a);
    return 2. / (q * q) * jacobian_numerator(a);
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

double G0(double t)
{
    return t * t * t - 2. * t * t + t;
}

double G0d(double t)
{
    return 3. * t * t - 4. * t + 1.;
}

double G1(double t)
{
    return t * t * t - t * t;
}

double G1d(double t)
{
    return 3. * t * t - 2. * t;
}

V<4> hermite(double t)
{
    return V<4>{F0(t), G0(t), F1(t), G1(t)};
}

V<4> hermite_d(double t)
{
    return V<4>{F0d(t), G0d(t), F1d(t), G1d(t)};
}


double ddivide(size_t a, size_t b)
{
    return static_cast<double>(a) / static_cast<double>(b);
}

std::pair<grid<V<3>>, grid<M<3, 2>>> calculate_coons(size_t res, const M<12, 4>& interp_data)
{

    std::vector<V<4>> fv;
    std::vector<M<3, 12>> fu;
    std::vector<V<4>> fvd;
    std::vector<M<3, 12>> fud;
    fv.reserve(res + 1);
    fu.reserve(res + 1);
    fvd.reserve(res + 1);
    fud.reserve(res + 1);
    for (int i = 0; i <= res; ++i) {
        double t = ddivide(i, res);
        V<4> f = hermite(t);
        V<4> fd = hermite_d(t);
        fv.push_back(f);
        fvd.push_back(fd);
        RV<4> ft = f.transpose();
        RV<4> fdt = fd.transpose();
        fu.push_back(inflate<3>(ft));
        fud.push_back(inflate<3>(fdt));
    }

    grid<V<3>> y{res + 1, res + 1};
    grid<M<3, 2>> dy_ds{res + 1, res + 1};

    for (auto [i, j] : range2d(res + 1, res + 1)) {
        y[i, j] << fu[i] * interp_data * fv[j];
        dy_ds[i, j] << fud[i] * interp_data * fv[j], fu[i] * interp_data * fvd[j];
    }

    return {std::move(y), std::move(dy_ds)};
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

constexpr double deriv_magnitude = 0.5;

grid<M<6, 2>> calculate_interp(const grid<V<3>>& p, const grid<V<3>>& n)
{
    grid<V<3>> a{3, 3};
    grid<V<3>> b{3, 3};

    for (auto [i, j] : range2d{3, 3}) {
        double h = p[i, j].dot(n[i, j]);
        a[i, j] = iota(n[i, j], h);
        M<4, 3> jac = iota_inv_jacobian(a[i, j]);
        M<3, 3> dn_dy = jac.topRows<3>();
        RV<3> dh_dy = jac.bottomRows<1>();
        b[i, j] = (p[i, j].transpose() * dn_dy - dh_dy).transpose().normalized();
    }

    grid<M<6, 2>> interp{3, 3};

    for (auto [i, j] : range2d{3, 3}) {
        V<3> gamma_star = calculate_gamma_star(a, i, j);
        V<3> gamma = gamma_star - b[i, j].dot(gamma_star) * b[i, j];
        gamma *= deriv_magnitude;

        V<3> delta_star = calculate_delta_star(a, i, j);
        V<3> delta = delta_star - b[i, j].dot(delta_star) * b[i, j];
        delta *= deriv_magnitude;

        interp[i, j] << a[i, j], delta, gamma, V<3>::Zero();
    }

    return interp;
}

std::array<grid<V<3>>, 2> from_isotropic_coons(const grid<V<3>>& y, const grid<M<3, 2>>& dy_ds)
{
    size_t res = y.n() - 1;
    grid<V<3>> n{res + 1, res + 1};
    grid<V<3>> x{res + 1, res + 1};

    for (auto [i, j] : indices(y)) {
        V<4> blaschke = iota_inv(y[i, j]);
        n[i, j] = blaschke.head<3>();
        double h = blaschke.tail<1>().value();

        M<4, 3> jacobian_numer = jacobian_numerator(y[i, j]);
        M<3, 2> MM = jacobian_numer.topLeftCorner<3, 2>();
        RV<2> kxy = jacobian_numer.bottomLeftCorner<1, 2>();

        M<2, 2> yxy = dy_ds[i, j].topRows<2>();
        RV<2> yz = dy_ds[i, j].bottomRows<1>();

        M<2, 2> yxyp;
        yxyp << yxy(1, 1), -yxy(0, 1), -yxy(1, 0), yxy(0, 0);

        double yxyd = yxy.determinant();

        double q = jacobian_q(y[i, j]);

        V<3> emb = 1. / (q * q * yxyd) * MM * (yxyd * kxy + yz * yxyp).transpose();

        x[i, j] = h * n[i, j] + emb;
    }

    return {std::move(x), std::move(n)};
}

int main()
{
    grid<V<3>> p{3, 3, { V<3>{0., 0., 0.},          V<3>{0., -11./72., -1./12.},    V<3>{0., -2./9., -1./3.},
                         V<3>{11./72., 0., 1./12.}, V<3>{7./36., -7./36., 0.},      V<3>{23./72., -11./36., -1./4.},
                         V<3>{2./9., 0., 1./3.},    V<3>{11./36., -23./72., 1./4.}, V<3>{5./9., -5./9., 0.} }};

    grid<V<3>> n_input{3, 3, { V<3>{0., 0., -1.},       V<3>{0., 4./5., -3./5.},    V<3>{0., 1., 0.},
                         V<3>{4./5., 0., -3./5.}, V<3>{2./3., 2./3., -1./3.}, V<3>{4./9., 8./9., 1./9.},
                         V<3>{1., 0., 0.},        V<3>{8./9., 4./9., 1./9.},  V<3>{2./3., 2./3., 1./3.} }};

    grid<M<6, 2>> interp = calculate_interp(p, n_input);

    M<12, 4> interp00;
    interp00 << interp[0, 0], interp[0, 1], interp[1, 0], interp[1, 1];

    size_t res = 100;

    auto [y, dy_ds] = calculate_coons(res, interp00);

    auto [x, n] = from_isotropic_coons(y, dy_ds);

    to_obj("out/coons00.obj", y);
    to_obj("out/n00.obj", n);
    to_obj("out/x00.obj", x);
}
