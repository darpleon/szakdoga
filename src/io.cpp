#include "io.hpp"

/*#include <sstream>*/
#include <fstream>
#include <print>
#include <iostream>

void print_obj_vertices(std::ostream& stream, const grid<V<3>>& vertices)
{
    for (const V<3>& v : vertices) {
        std::println(stream, "v {} {} {}", v[0], v[1], v[2]);
    }
}

void print_obj_normals(std::ostream& stream, const grid<V<3>>& normals)
{
    for (const V<3>& n : normals) {
        std::println(stream, "vn {} {} {}", n[0], n[1], n[2]);
    }
}

void print_obj_faces(std::ostream& stream, size_t n, size_t m)
{
    for (auto [i, j] : range2d{n - 1, m - 1}) {
        size_t tl = m * i + j + 1; 
        size_t tr = tl + 1;
        size_t bl = tl + m;
        size_t br = bl + 1;
        std::println(stream, "f {} {} {}", tl, bl, tr);
        std::println(stream, "f {} {} {}", tr, bl, br);
    }
}

void print_obj_faces_normals(std::ostream& stream, size_t n, size_t m, size_t start)
{
    for (auto [i, j] : range2d{n - 1, m - 1}) {
        size_t tl = start + 1 + m * i + j; 
        size_t tr = tl + 1;
        size_t bl = tl + m;
        size_t br = bl + 1;
        std::println(stream, "f {}//{} {}//{} {}//{}", tl, tl, bl, bl, tr, tr);
        std::println(stream, "f {}//{} {}//{} {}//{}", tr, tr,  bl, bl, br, br);
    }
}

void to_obj(std::string filename, const grid<V<3>>& vertices)
{
    std::stringstream vertex_stream;
    std::stringstream face_stream;

    print_obj_vertices(vertex_stream, vertices);

    size_t n = vertices.n();
    size_t m = vertices.m();
    print_obj_faces(face_stream, n, m);

    std::ofstream output(filename);
    output << vertex_stream.rdbuf() << '\n' << face_stream.rdbuf();
}

void to_obj(std::string filename, const grid<V<3>>& vertices, const grid<V<3>>& normals)
{
    std::stringstream vertex_stream;
    std::stringstream normal_stream;
    std::stringstream face_stream;

    print_obj_vertices(vertex_stream, vertices);
    print_obj_normals(normal_stream, normals);

    size_t n = vertices.n();
    size_t m = vertices.m();
    print_obj_faces_normals(face_stream, n, m);

    std::ofstream output{filename};
    output << vertex_stream.rdbuf() << '\n';
    output << normal_stream.rdbuf() << '\n';
    output << face_stream.rdbuf();
}

void to_obj(std::string filename, const std::vector<grid<V<3>>>& patches)
{
    std::stringstream vertex_stream;
    std::stringstream face_stream;

    size_t group_id = 0;
    size_t index_start = 0;
    for (const auto& vertices : patches) {
        print_obj_vertices(vertex_stream, vertices);

        size_t n = vertices.n();
        size_t m = vertices.m();
        std::println(face_stream, "g {}", group_id);
        print_obj_faces_normals(face_stream, n, m, index_start);
        ++group_id;
        index_start += n * m;
    }

    std::ofstream output{filename};
    output << vertex_stream.rdbuf() << '\n';
    output << face_stream.rdbuf();
}

void to_obj(std::string filename, const std::vector<std::array<grid<V<3>>, 2>>& patches)
{
    std::stringstream vertex_stream;
    std::stringstream normal_stream;
    std::stringstream face_stream;

    size_t group_id = 0;
    size_t index_start = 0;
    for (const auto& [vertices, normals] : patches) {
        print_obj_vertices(vertex_stream, vertices);
        print_obj_normals(normal_stream, normals);

        size_t n = vertices.n();
        size_t m = vertices.m();
        std::println(face_stream, "g {}", group_id);
        print_obj_faces_normals(face_stream, n, m, index_start);
        ++group_id;
        index_start += n * m;
    }

    std::ofstream output{filename};
    output << vertex_stream.rdbuf() << '\n';
    output << normal_stream.rdbuf() << '\n';
    output << face_stream.rdbuf();
    // std::cout << face_stream.rdbuf();
}
