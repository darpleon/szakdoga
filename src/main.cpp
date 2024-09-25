#include <format>
#include <iostream>
#include <eigen3/Eigen/Core>
#include "Rational.hpp"

using v3 = Eigen::Vector3d;

struct isotropic_point;


struct isotropic_point
{
    v3 position;
    v3 direction_u;
    v3 direction_v;

    void project_direction()
    {
        // direction_u = direction_u - normal * direction_u.dot(normal);
        // direction_v = direction_v - normal * direction_v.dot(normal);
    }
};

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
        result.position = v3{normal.x(), normal.y(), h()} * 1.0 / (1.0 + normal.z());
        return result;
    }
};

std::string to_string(v3 v)
{
    return std::format("({:.4f}, {:.4f}, {:.4f})", v[0], v[1], v[2]);
}

int main()
{
    std::array<std::array<primal_point, 2>, 2> primal;

    primal[0][0].position = v3{-2.0, 1.0, 0.0};
    primal[1][0].position = v3{2.0, -3.0, 2.0};
    primal[0][1].position = v3{0.0, 4.0, -1.0};
    primal[1][1].position = v3{4.0, 3.0, 1.0};

    primal[0][0].normal = v3{0.0, 0.0, 1.0};
    primal[1][0].normal = v3{-2.0, 3.0, 6.0} * 1.0 / 7.0;
    primal[0][1].normal = v3{2.0, 10.0, 11.0} * 1.0 / 15.0;
    primal[1][1].normal = v3{8.0, 11.0, 16.0} * 1.0 / 21.0;

    std::array<std::array<isotropic_point, 2>, 2> isotropic;

    for (int u = 0; u < 2; u++) {
        for (int v = 0; v < 2; v++) {
            isotropic[u][v] = primal[u][v].to_isotropic();
            std::cout << std::format("primal[{}][{}] = {}\n", u, v, to_string(primal[u][v].position));
            std::cout << std::format("dual = {}, h = {:.4f}\n", to_string(primal[u][v].normal), primal[u][v].h());
            std::cout << std::format("isotropic[{}][{}] = {}\n", u, v, to_string(isotropic[u][v].position));
            std::cout << "========================================\n";
        }
    }

    for (int u = 0; u < 2; u++) {
        for (int v = 0; v < 2; v++) {
            // double h = isotropic[u][v].h();

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
            std::cout << std::format("pos = {}\n", to_string(isotropic[u][v].position));
            // std::cout << std::format("n = {}\n", to_string(isotropic[u][v].normal));
            std::cout << std::format("direction u = {}\n", to_string(isotropic[u][v].direction_u));
            std::cout << std::format("direction v = {}\n", to_string(isotropic[u][v].direction_v));
            std::cout << "================================================================\n";
        }
    }


}
