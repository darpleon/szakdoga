#include "io.hpp"

/*#include <sstream>*/
#include <fstream>

void to_obj(std::string filename, const grid<V<3>>& vertices)
{
    std::stringstream vertex_stream;
    std::stringstream face_stream;

    for (const V<3>& v : vertices) {
        std::println(vertex_stream, "v {} {} {}", v[0], v[1], v[2]);
    }
    size_t n = vertices.n();
    size_t m = vertices.m();
    for (auto [i, j] : range2d{n - 1, m - 1}) {
        size_t tl = m * i + j + 1; 
        size_t tr = tl + 1;
        size_t bl = tl + m;
        size_t br = bl + 1;
        std::println(face_stream, "f {} {} {}", tl, tr, bl);
        std::println(face_stream, "f {} {} {}", tr, bl, br);
    }

    std::ofstream output(filename);
    output << vertex_stream.rdbuf() << face_stream.rdbuf();
}
