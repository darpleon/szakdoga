#include <algorithm>
#include <array>
#include <concepts>
#include <format>
#include <iostream>
#include <eigen3/Eigen/Core>
#include "Rational.hpp"

using v3 = Eigen::Vector3d;
using v4 = Eigen::Vector4d;
using m3 = Eigen::Matrix3d;
using m34 = Eigen::Matrix<double, 3, 4>;
using rv3 = Eigen::RowVector3d;

static Variable x, y, z, u, v;

struct isotropic_point
{
    static std::array<std::vector<Rational<double>>, 4> iota_inv_jacobian;

    v3 position;
    v3 primal_position;
    v3 direction_u;
    v3 direction_v;

    void project_direction()
    {
        m34 jacobian_transpose{};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                jacobian_transpose(j, i) = iota_inv_jacobian[i][j].evaluate<double>({{x, position.x()}, {y, position.y()}, {z, position.z()}});
            }
        }
        v4 p_star{};
        p_star << primal_position, -1.0;

        v3 n_star = jacobian_transpose * p_star;

        direction_u = direction_u - n_star * direction_u.dot(n_star);
        direction_v = direction_v - n_star * direction_v.dot(n_star);
    }
};

std::array<std::vector<Rational<double>>, 4> isotropic_point::iota_inv_jacobian{};

struct primal_point
{
    v3 position;
    v3 normal;

    double h() const
    {
        return position.dot(normal);
    }

    isotropic_point to_isotropic() const
    {
        isotropic_point result;
        result.position = v3{normal.x(), normal.y(), h()} * 1.0 / (1.0 - normal.z());
        result.primal_position = position;
        return result;
    }
};

std::string to_string(v3 v)
{
    return std::format("({:.4f}, {:.4f}, {:.4f})", v[0], v[1], v[2]);
}

v3 operator*(v3 lhs, v3 rhs) {
    return v3(lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]);
}

int main()
{
    std::array<std::array<primal_point, 2>, 2> primal;

    primal[0][0].position = v3{-2.0, 1.0, 0.0};
    primal[1][0].position = v3{2.0, -3.0, 2.0};
    primal[0][1].position = v3{0.0, 4.0, -1.0};
    primal[1][1].position = v3{4.0, 3.0, 1.0};

    primal[0][0].normal = v3{0.0, 0.0, -1.0};
    primal[1][0].normal = v3{-2.0, 3.0, -6.0} * 1.0 / 7.0;
    primal[0][1].normal = v3{2.0, 10.0, -11.0} * 1.0 / 15.0;
    primal[1][1].normal = v3{8.0, 11.0, -16.0} * 1.0 / 21.0;

    std::array<std::array<isotropic_point, 2>, 2> isotropic;

    Polynomial<double> iota_inv_denom = 1.0 + (x^2) + (y^2);

    std::array<Rational<double>, 4> iota_inv{2.0*x, 2.0*y, 1.0 - (x^2) - (y^2), 2.0*z};
    for (auto& func : iota_inv) {
        func /= iota_inv_denom;
    }

    for (size_t i = 0; i < 4; ++i) {
        isotropic_point::iota_inv_jacobian[i] = iota_inv[i].derivative({x, y, z});
    }

    for (int u = 0; u < 2; u++) {
        for (int v = 0; v < 2; v++) {
            isotropic[u][v] = primal[u][v].to_isotropic();
        }
    }

    for (int u = 0; u < 2; u++) {
        for (int v = 0; v < 2; v++) {
            isotropic[u][v].direction_u = isotropic[1][v].position - isotropic[0][v].position;
            isotropic[u][v].direction_v = isotropic[u][1].position - isotropic[u][0].position;

            if (u == 1) {
                isotropic[u][v].direction_u *= 2.0;
            }
            if (v == 1) {
                isotropic[u][v].direction_v *= 2.0;
            }
            isotropic[u][v].project_direction();
            
            std::cout << std::format("u = {}, v = {}\n", u, v);
            std::cout << std::format("primal = {}\n", to_string(primal[u][v].position));
            std::cout << std::format("dual = {}, h = {:.4f}\n", to_string(primal[u][v].normal), primal[u][v].h());
            std::cout << std::format("isotropic = {}\n", to_string(isotropic[u][v].position));
            std::cout << std::format("direction u = {}\n", to_string(isotropic[u][v].direction_u));
            std::cout << std::format("direction v = {}\n", to_string(isotropic[u][v].direction_v));
            std::cout << "================================================================\n";
        }
    }

    Rational<double> F0u = 2.0*(u^3) - 3.0*(u^2) + 1;
    Rational<double> F1u = -2.0*(u^3) + 3.0*(u^2);
    Rational<double> G0u = 1.0*(u^3) - 2.0*(u^2) + u;
    Rational<double> G1u = 1.0*(u^3) - (u^2);

    Rational<double> F0v = 2.0*(v^3) - 3.0*(v^2) + 1;
    Rational<double> F1v = -2.0*(v^3) + 3.0*(v^2);
    Rational<double> G0v = 1.0*(v^3) - 2.0*(v^2) + v;
    Rational<double> G1v = 1.0*(v^3) - (v^2);

    std::array<Rational<double>, 3> c0u;
    std::array<Rational<double>, 3> c1u;
    std::array<Rational<double>, 3> c0v;
    std::array<Rational<double>, 3> c1v;

    for (int i = 0; i < 3; ++i) {
        c0u[i] = isotropic[0][0].position[i]    * F0u + isotropic[1][0].position[i] * F1u +
                 isotropic[0][0].direction_u[i] * G0u + isotropic[1][0].direction_u[i] * G1u;
        c1u[i] = isotropic[0][1].position[i]    * F0u + isotropic[1][1].position[i] * F1u +
                 isotropic[0][1].direction_u[i] * G0u + isotropic[1][1].direction_u[i] * G1u;
        c0v[i] = isotropic[0][0].position[i]    * F0v + isotropic[0][1].position[i] * F1v +
                 isotropic[0][0].direction_v[i] * G0v + isotropic[0][1].direction_v[i] * G1v;
        c1v[i] = isotropic[1][0].position[i]    * F0v + isotropic[1][1].position[i] * F1v +
                 isotropic[1][0].direction_v[i] * G0v + isotropic[1][1].direction_v[i] * G1v;
    }

    /*std::cout << "c0u:\n";*/
    /*for (int i = 0; i < 3; ++i) {*/
    /*    std::cout << c0u[i] << "\n";*/
    /*}*/

    std::array<Rational<double>, 3> isotropic_coons;

    /*std::cout << "isotropic coons:\n";*/
    for (int i = 0; i < 3; ++i) {
        isotropic_coons[i] = c0u[i] * F0v + c1u[i] * F1v + c0v[i] * F0u + c1v[i] * F1u
                           + -1.0 * (isotropic[0][0].position[i] * F0u * F0v
                                   + isotropic[1][0].position[i] * F1u * F0v
                                   + isotropic[0][1].position[i] * F0u * F1v
                                   + isotropic[1][1].position[i] * F1u * F1v);
        /*std::cout << isotropic_coons[i] << "\n";*/
    }

    std::array<Rational<double>, 4> dual_result;

    std::cout << "result in dual (nx, ny, nz, h)\n\n";
    for (int i = 0; i < 4; ++i) {
        dual_result[i] = iota_inv[i].evaluate<Rational<double>>({
                {x, isotropic_coons[0]},
                {y, isotropic_coons[1]},
                {z, isotropic_coons[2]}
                });
        /*std::cout << dual_result[i].evaluate<double>({{u, 0.5}, {v, 0.5}}) << "\n";*/
        std::cout << dual_result[i] << "\n\n";
    }

    std::array<Rational<double>, 3> dndu;
    std::array<Rational<double>, 3> dndv;
    
    for (int i = 0; i < 3; ++i) {
        dndu[i] = dual_result[i].derivative(u);
        dndv[i] = dual_result[i].derivative(v);
    }

    Rational<double> dhdu = dual_result[3].derivative(u);
    Rational<double> dhdv = dual_result[3].derivative(v);

    Rational<double> dndudot{};
    Rational<double> dndvdot{};

    for (int i = 0; i < 3; ++i) {
        dndudot += dndu[i] * dndu[i];
        dndvdot += dndv[i] * dndv[i];
    }

    Rational<double> hu_star = dhdu / dndudot;
    Rational<double> hv_star = dhdv / dndvdot;

    std::array<Rational<double>, 3> primal_result;

    std::cout << "result in primal (x, y, z)\n\n";
    for (int i = 0; i < 3; ++i) {
        primal_result[i] = dual_result[3] * dual_result[i] + hu_star * dndu[i] + hv_star * dndv[i];
        std::cout << primal_result[i] << "\n\n";
    }
}
