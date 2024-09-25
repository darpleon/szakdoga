#include <format>
#include <iostream>
#include <eigen3/Eigen/Core>
#include "Rational.hpp"

using v3 = Eigen::Vector3d;

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

struct point_data
{
    v3 position;
    v3 normal;
    v3 direction_u;
    v3 direction_v;

    void project_direction()
    {
        std::cout << "projectin\n";
        direction_u = direction_u - normal * direction_u.dot(normal);
        direction_v = direction_v - normal * direction_v.dot(normal);
    }

    double h() const
    {
        return position.dot(normal);
    }

    point_data to_isometric() const
    {
        point_data result;
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
    std::array<std::array<point_data, 2>, 2> input;

    input[0][0].position = v3{-2.0, 1.0, 0.0};
    input[1][0].position = v3{2.0, -3.0, 2.0};
    input[0][1].position = v3{0.0, 4.0, -1.0};
    input[1][1].position = v3{4.0, 3.0, 1.0};

    input[0][0].normal = v3{0.0, 0.0, 1.0};
    input[1][0].normal = v3{-2.0, 3.0, 6.0} * 1.0 / 7.0;
    input[0][1].normal = v3{2.0, 10.0, 11.0} * 1.0 / 15.0;
    input[1][1].normal = v3{8.0, 11.0, 16.0} * 1.0 / 21.0;

    for (int u = 0; u < 2; u++) {
        for (int v = 0; v < 2; v++) {
            double h = input[u][v].h();

            input[u][v].direction_u = input[1][v].position - input[0][v].position;
            input[u][v].direction_v = input[u][1].position - input[u][0].position;

            if (u == 1) {
                input[u][v].direction_u *= 2.0;
            }
            if (v == 1) {
                input[u][v].direction_v *= 2.0;
            }

            input[u][v].project_direction();
            
            std::cout << std::format("u = {}, v = {}\n", u, v);
            std::cout << std::format("pos = {}\n", to_string(input[u][v].position));
            std::cout << std::format("n = {}\n", to_string(input[u][v].normal));
            std::cout << std::format("direction u = {}\n", to_string(input[u][v].direction_u));
            std::cout << std::format("direction v = {}\n", to_string(input[u][v].direction_v));
            std::cout << "================================================================\n";
        }
    }


}
