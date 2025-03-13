#pragma once

#include "matrix_utility.hpp"
#include "grid.hpp"

namespace numeric_implementation {

double ddivide(size_t a, size_t b);

grid<M<6, 2>> calculate_interp(const grid<V<3>>& p, const grid<V<3>>& n);

}
