#include <algorithm>
#include <array>
#include <concepts>
#include <format>
#include <iostream>
#include <eigen3/Eigen/Core>
#include "Polynomial.hpp"
#include "Rational.hpp"
#include <fstream>
#include <sstream>
#include <string>

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

void to_obj_single(std::string filename, Rational<double> surface, int res) {
    std::ofstream output(filename);
    std::stringstream vertex_stream;
    std::stringstream face_stream;

    double step = 1.0f / (res - 1);

    double u_val = 0.0;
    double v_val = 0.0;
    for (int i = 0; i < res; ++i) {
        for (int j = 0; j < res; ++j) {
            double surface_val = surface.evaluate<double>({{u, u_val}, {v, v_val}});
            vertex_stream << std::format("v {} {} {}\n", u_val, v_val, surface_val);
            u_val += step;
        }
        u_val = 0.0;
        v_val += step;
    }

    for (int i = 0; i < res - 1; ++i) {
        for (int j = 0; j < res - 1; ++j) {
            int bl = res * i + j + 1;
            int br = bl + 1;
            int tl = bl + res;
            int tr = tl + 1;
            face_stream << std::format("f {} {} {}\n", bl, br, tl);
            face_stream << std::format("f {} {} {}\n", tl, br, tr);
        }
    }

    output << vertex_stream.rdbuf() << "\n" << face_stream.rdbuf();
}

void to_obj(std::string filename, std::array<Polynomial<double>, 3> surface, int res) {
    std::ofstream output(filename);
    std::stringstream vertex_stream;
    std::stringstream normal_stream;
    std::stringstream face_stream;

    double step = 1.0f / (res - 1);

    double u_val = 0.0;
    double v_val = 0.0;
    for (int i = 0; i < res; ++i) {
        for (int j = 0; j < res; ++j) {
            std::array<double, 3> surface_val;
            std::array<double, 3> normal_val;
            for (int c = 0; c < 3; ++c) {
                surface_val[c] = surface[c].evaluate<double>({{u, u_val}, {v, v_val}});
            }
            vertex_stream << std::format("v {} {} {}\n", surface_val[0], surface_val[1], surface_val[2]);
            u_val += step;
        }
        u_val = 0.0;
        v_val += step;
    }

    for (int i = 0; i < res - 1; ++i) {
        for (int j = 0; j < res - 1; ++j) {
            int bl = res * i + j + 1;
            int br = bl + 1;
            int tl = bl + res;
            int tr = tl + 1;
            face_stream << std::format("f {} {} {}\n", bl, br, tl);
            face_stream << std::format("f {} {} {}\n", tl, br, tr);
        }
    }

    output << vertex_stream.rdbuf() << "\n" << "\n" << face_stream.rdbuf();
}

void to_obj(std::string filename, std::array<Rational<double>, 3> surface, int res) {
    std::ofstream output(filename);
    std::stringstream vertex_stream;
    std::stringstream normal_stream;
    std::stringstream face_stream;

    double step = 1.0f / (res - 1);

    double u_val = 0.0;
    double v_val = 0.0;
    for (int i = 0; i < res; ++i) {
        for (int j = 0; j < res; ++j) {
            std::array<double, 3> surface_val;
            std::array<double, 3> normal_val;
            for (int c = 0; c < 3; ++c) {
                surface_val[c] = surface[c].evaluate<double>({{u, u_val}, {v, v_val}});
            }
            vertex_stream << std::format("v {} {} {}\n", surface_val[0], surface_val[1], surface_val[2]);
            u_val += step;
        }
        u_val = 0.0;
        v_val += step;
    }

    for (int i = 0; i < res - 1; ++i) {
        for (int j = 0; j < res - 1; ++j) {
            int bl = res * i + j + 1;
            int br = bl + 1;
            int tl = bl + res;
            int tr = tl + 1;
            face_stream << std::format("f {} {} {}\n", bl, br, tl);
            face_stream << std::format("f {} {} {}\n", tl, br, tr);
        }
    }

    output << vertex_stream.rdbuf() << "\n" << "\n" << face_stream.rdbuf();
}

void to_obj_normal(std::string filename, std::array<Rational<double>, 3> surface, std::array<Rational<double>, 3> normal, int res) {
    std::ofstream output(filename);
    std::stringstream vertex_stream;
    std::stringstream normal_stream;
    std::stringstream face_stream;

    double step = 1.0f / (res - 1);

    double u_val = 0.0;
    double v_val = 0.0;
    for (int i = 0; i < res; ++i) {
        for (int j = 0; j < res; ++j) {
            std::array<double, 3> surface_val;
            std::array<double, 3> normal_val;
            for (int c = 0; c < 3; ++c) {
                surface_val[c] = surface[c].evaluate<double>({{u, u_val}, {v, v_val}});
                normal_val[c] = normal[c].evaluate<double>({{u, u_val}, {v, v_val}});
            }
            vertex_stream << std::format("v {} {} {}\n", surface_val[0], surface_val[1], surface_val[2]);
            normal_stream << std::format("vn {} {} {}\n", normal_val[0], normal_val[1], normal_val[2]);
            u_val += step;
        }
        u_val = 0.0;
        v_val += step;
    }

    for (int i = 0; i < res - 1; ++i) {
        for (int j = 0; j < res - 1; ++j) {
            int bl = res * i + j + 1;
            int br = bl + 1;
            int tl = bl + res;
            int tr = tl + 1;
            face_stream << std::format("f {}//{} {}//{} {}//{}\n", bl, bl, br, br, tl, tl);
            face_stream << std::format("f {}//{} {}//{} {}//{}\n", tl, tl, br, br, tr, tr);
        }
    }

    output << vertex_stream.rdbuf() << "\n" << normal_stream.rdbuf() << "\n" << face_stream.rdbuf();
}

v3 operator*(v3 lhs, v3 rhs) {
    return v3(lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]);
}

int main()
{
    std::array<std::array<primal_point, 2>, 2> primal;

    primal[0][0].position = v3{0.0, 0.0, 0.0};
    primal[1][0].position = v3{0.0, -0.152778, -0.083333};
    primal[0][1].position = v3{0.152778, 0.0, 0.833333};
    primal[1][1].position = v3{0.194444, -0.194444, 0.0};

    primal[0][0].normal = v3{0.0, 0.0, -1.0};
    primal[1][0].normal = v3{0.0, 4.0, -3.0} * 1.0 / 5.0;
    primal[0][1].normal = v3{4.0, 0.0, -3.0} * 1.0 / 5.0;
    primal[1][1].normal = v3{2.0, 2.0, -1.0} * 1.0 / 3.0;

    /*primal[0][0].position = v3{-2.0, 1.0, 0.0};*/
    /*primal[1][0].position = v3{2.0, -3.0, 2.0};*/
    /*primal[0][1].position = v3{0.0, 4.0, -1.0};*/
    /*primal[1][1].position = v3{4.0, 3.0, 1.0};*/
    /**/
    /*primal[0][0].normal = v3{0.0, 0.0, -1.0};*/
    /*primal[1][0].normal = v3{-2.0, 3.0, -6.0} * 1.0 / 7.0;*/
    /*primal[0][1].normal = v3{2.0, 10.0, -11.0} * 1.0 / 15.0;*/
    /*primal[1][1].normal = v3{8.0, 11.0, -16.0} * 1.0 / 21.0;*/

    std::array<std::array<isotropic_point, 2>, 2> isotropic;

    Polynomial<double> iota_inv_denom = 1.0 + (x^2) + (y^2);

    std::array<Polynomial<double>, 4> iota_inv_numer{2.0*x, 2.0*y, -1.0 + (x^2) + (y^2), 2.0*z};

    std::array<Rational<double>, 4> iota_inv{};
    for (int i = 0; i < 3; i++) {
        iota_inv[i] = iota_inv_numer[i] / iota_inv_denom;
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

    Polynomial<double> F0u = 2.0*(u^3) - 3.0*(u^2) + 1;
    Polynomial<double> F1u = -2.0*(u^3) + 3.0*(u^2);
    Polynomial<double> G0u = 1.0*(u^3) - 2.0*(u^2) + u;
    Polynomial<double> G1u = 1.0*(u^3) - (u^2);

    Polynomial<double> F0v = 2.0*(v^3) - 3.0*(v^2) + 1;
    Polynomial<double> F1v = -2.0*(v^3) + 3.0*(v^2);
    Polynomial<double> G0v = 1.0*(v^3) - 2.0*(v^2) + v;
    Polynomial<double> G1v = 1.0*(v^3) - (v^2);

    std::array<Polynomial<double>, 3> c0u{};
    std::array<Polynomial<double>, 3> c1u{};
    std::array<Polynomial<double>, 3> c0v{};
    std::array<Polynomial<double>, 3> c1v{};

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

    std::array<Polynomial<double>, 3> isotropic_coons{};

    /*std::cout << "isotropic coons:\n";*/
    for (int i = 0; i < 3; ++i) {
        isotropic_coons[i] = c0u[i] * F0v + c1u[i] * F1v + c0v[i] * F0u + c1v[i] * F1u
                           + -1.0 * (isotropic[0][0].position[i] * F0u * F0v
                                   + isotropic[1][0].position[i] * F1u * F0v
                                   + isotropic[0][1].position[i] * F0u * F1v
                                   + isotropic[1][1].position[i] * F1u * F1v);
        /*std::cout << isotropic_coons[i] << "\n";*/
    }

    Polynomial<double> d = iota_inv_denom.evaluate<Polynomial<double>>({
            {x, isotropic_coons[0]},
            {y, isotropic_coons[1]},
            {z, isotropic_coons[2]}
            });

    std::array<Polynomial<double>, 3> m{};
    std::array<Rational<double>, 3> n{};
    for (int i = 0; i < 3; ++i) {
        m[i] = iota_inv_numer[i].evaluate<Polynomial<double>>({
                {x, isotropic_coons[0]},
                {y, isotropic_coons[1]},
                {z, isotropic_coons[2]}
                });
        n[i] = m[i] / d;
    }

    to_obj_normal("normal.obj", n, n, 20);

    Polynomial<double> k = iota_inv_numer[3].evaluate<Polynomial<double>>({
            {x, isotropic_coons[0]},
            {y, isotropic_coons[1]},
            {z, isotropic_coons[2]}
            });
    Rational<double> h = k / d;

    Polynomial<double> d2 = d * d;
    Polynomial<double> d4 = d2 * d2;

    to_obj_single("support.obj", h, 20);

    std::array<Polynomial<double>, 3> mu{};
    std::array<Polynomial<double>, 3> mv{};
    std::array<Rational<double>, 3> nu{};
    std::array<Rational<double>, 3> nv{};
    Polynomial<double> du_denom = d.derivative(u);
    Polynomial<double> dv_denom = d.derivative(v);

    for (int i = 0; i < 3; ++i) {
        mu[i] = m[i].derivative(u) * d - m[i] * du_denom;
        mv[i] = m[i].derivative(v) * d - m[i] * dv_denom;
        nu[i] = mu[i] / d2;
        nv[i] = mv[i] / d2;
    }

    to_obj("nu.obj", nu, 20);
    std::cout << "nu\n";

    Polynomial<double> ku = k.derivative(u) * d - k * du_denom;
    Polynomial<double> kv = k.derivative(v) * d - k * dv_denom;
    Rational<double> hu = ku / d2;
    Rational<double> hv = kv / d2;
    to_obj_single("hu.obj", hu, 40);
    std::cout << "hu\n";

    Polynomial<double> muu{};
    Polynomial<double> mvv{};
    Polynomial<double> muv{};

    for (int i = 0; i < 3; ++i) {
        muu += mu[i] * mu[i];
        mvv += mv[i] * mv[i];
        muv += mu[i] * mv[i];
    }

    Rational<double> nuu = muu / d4;
    Rational<double> nvv = mvv / d4;
    Rational<double> nuv = muv / d4;
    to_obj_single("nuu.obj", nuu, 40);
    std::cout << "nuu\n";
    to_obj_single("nvv.obj", nvv, 40);
    std::cout << "nvv\n";

    Polynomial<double> k_alpha = mvv * ku - muv * kv;
    to_obj_single("alpha.obj", k_alpha, 40);
    std::cout << "alpha\n";
    Polynomial<double> k_beta = muu * kv - muv * ku;
    std::cout << "kv*\n";
    Polynomial<double> m_star = muu * mvv - muv * muv;
    std::cout << "m*\n";

    to_obj_single("star.obj", m_star, 40);
    std::cout << "star\n";

    Polynomial<double> result_denom = d2 * m_star;

    std::array<Polynomial<double>, 3> result_numer{};
    std::array<Rational<double>, 3> result{};

    for (int i = 0; i < 3; ++i) {
        result_numer[i] = k * m_star * m[i]
                        + d2 * (k_alpha * mu[i] + k_beta * mv[i]);
        result[i] = result_numer[i] / result_denom;
        std::cout << "one coordinate down\n";
    }


    std::cout << "yay\n";

    to_obj_normal("output.obj", result, n, 50);

    /*std::cout << result_denom.terms().size() << "\n";*/
    /*std::cout << result_numer[0].terms().size() << "\n";*/
    /*std::cout << result_numer[2].terms().size() << "\n";*/


}
