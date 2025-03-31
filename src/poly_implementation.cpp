#include "Polynomial.hpp"
#include "Rational.hpp"
#include "grid.hpp"
#include "basic_implementation.hpp"
#include "io.hpp"
#include <iostream>

namespace ni = numeric_implementation;

using PD = Polynomial<double>;

namespace function_implementation {

template <size_t S>
std::array<PD, S> deriv(const std::array<PD, S>& p, Variable t)
{
    std::array<PD, S> result;
    for (size_t i = 0; i < S; ++i) {
        result[i] = p[i].derivative(t);
    }
    return result;
}

std::array<Polynomial<double>, 4> hermite(Variable t)
{
    return {2. * (t^3)  - 3. * (t^2) + 1.,
            1. * (t^3) - 2. * (t^2) + t,
            -2. * (t^3) + 3. * (t^2),
            1. * (t^3) - 1. * (t^2) };
}

std::array<Polynomial<double>, 3> calculate_coons(const M<12, 4>& interp_data, Variable u, Variable v)
{
    std::array<PD, 4> Fu = hermite(u);
    std::array<PD, 4> Fv = hermite(v);

    std::array<PD, 3> C{0.};

    for (auto [i, j] : range2d{4, 4}) {
        for (size_t k = 0; k < 3; ++k) {
            C[k] += interp_data(3 * i + k, j) * Fu[i] * Fv[j];
        }
    }

    return C;
}

std::array<std::array<Rational<double>, 3>, 2> from_isotropic_coons(std::array<PD, 3> y, Variable u, Variable v)
{
    std::array<PD, 3> deriv_u = deriv(y, u);
    std::array<PD, 3> deriv_v = deriv(y, v);

    PD cx2 = y[0] * y[0];
    PD cy2 = y[1] * y[1];
    PD cxy_prod = y[0] * y[1];
    PD square_sum = cx2 + cy2;
    PD square_diff = cx2 - cy2;

    std::array<PD, 3> Mu = {1. - square_diff, -2. * cxy_prod, 2. * y[0]};
    std::array<PD, 3> Mv = {-2. * cxy_prod, 1. + square_diff, 2. * y[1]};
    std::array<PD, 2> kxy = {-2. * y[0] * y[2], -2. * y[1] * y[2]};

    std::array<PD, 3> m = {2. * y[0], 2. * y[1], -1. + square_sum};
    PD k = 2. * y[2];

    PD q = 1. + square_sum;
    PD Yxy_det = deriv_u[0] * deriv_v[1] - deriv_v[0] * deriv_u[1];

    std::array<PD, 2> mul;
    mul[0] = Yxy_det * kxy[0] + deriv_u[2] * deriv_v[1] - deriv_v[2] * deriv_u[1];
    mul[1] = Yxy_det * kxy[1] - deriv_u[2] * deriv_v[0] + deriv_v[2] * deriv_u[0];

    std::array<PD, 3> emb_numer;
    for (size_t i = 0; i < 3; ++i) {
        emb_numer[i] = Mu[i] * mul[0] + Mv[i] * mul[1];
    }

    std::array<PD, 3> pl_numer;
    for (size_t i = 0; i < 3; ++i) {
        pl_numer[i] = m[i] * k * Yxy_det;
    }

    std::array<PD, 3> numer;
    for (size_t i = 0; i < 3; ++i) {
        numer[i] = pl_numer[i] + emb_numer[i];
    }

    PD q2 = q * q;
    PD denom = q2 * Yxy_det;

    std::array<Rational<double>, 3> n;
    for (size_t i = 0; i < 3; ++i) {
        n[i] = m[i] / q;
    }

    std::cout << numer[0].terms().size() << "\n";

    std::array<Rational<double>, 3> x;
    for (size_t i = 0; i < 3; ++i) {
        x[i] = numer[i] / denom;
    }

    return {x, n};
}

}

namespace fi = function_implementation;

int main()
{
    grid<V<3>> p{3, 3, { V<3>{0., 0., 0.},          V<3>{0., -11./72., -1./12.},    V<3>{0., -2./9., -1./3.},
                         V<3>{11./72., 0., 1./12.}, V<3>{7./36., -7./36., 0.},      V<3>{23./72., -11./36., -1./4.},
                         V<3>{2./9., 0., 1./3.},    V<3>{11./36., -23./72., 1./4.}, V<3>{5./9., -5./9., 0.} }};

    grid<V<3>> n_input{3, 3, { V<3>{0., 0., -1.},       V<3>{0., 4./5., -3./5.},    V<3>{0., 1., 0.},
                         V<3>{4./5., 0., -3./5.}, V<3>{2./3., 2./3., -1./3.}, V<3>{4./9., 8./9., 1./9.},
                         V<3>{1., 0., 0.},        V<3>{8./9., 4./9., 1./9.},  V<3>{2./3., 2./3., 1./3.} }};

    grid<M<6, 2>> interp_data = ni::calculate_interp(p, n_input);

    std::vector<std::array<grid<V<3>>, 2>> patches;
    for (auto [i, j] : range2d{2, 2}) {
        M<12, 4> interp;
        interp << interp_data[i + 0, j + 0], interp_data[i + 0, j + 1],
                  interp_data[i + 1, j + 0], interp_data[i + 1, j + 1];

        Variable u, v;

        std::array<PD, 3> c = fi::calculate_coons(interp, u, v);
        auto [x, n] = fi::from_isotropic_coons(c, u, v);

        size_t res = 10;

        grid<V<3>> x_val{res + 1, res + 1};
        grid<V<3>> n_val{res + 1, res + 1};

        for (auto [i, j] : range2d{res + 1, res + 1}) {
            double u_val = ni::ddivide(i, res);
            double v_val = ni::ddivide(j, res);
            for(size_t k = 0; k < 3; ++k) {
                x_val[i, j][k] = x[k].evaluate<double>({{u, u_val}, {v, v_val}});
                n_val[i, j][k] = n[k].evaluate<double>({{u, u_val}, {v, v_val}});
            }
        }

        patches.push_back({std::move(x_val), std::move(n_val)});
    }

    to_obj("out/x00_poly.obj", patches);


    return 0;
}

