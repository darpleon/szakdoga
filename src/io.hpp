#pragma once

#include "matrix_utility.hpp"
#include "grid.hpp"
#include <string>

void print_obj_vertices(std::ostream& stream, const grid<V<3>>& vertices);

void print_obj_normals(std::ostream& stream, const grid<V<3>>& normals);

void print_obj_faces(std::ostream& stream, size_t n, size_t m);

void print_obj_faces_normals(std::ostream& stream, size_t n, size_t m, size_t start = 0);

void to_obj(std::string filename, const grid<V<3>>& vertices);

void to_obj(std::string filename, const grid<V<3>>& vertices, const grid<V<3>>& normals);

void to_obj(std::string filename, const std::vector<std::array<grid<V<3>>, 2>>& points);
