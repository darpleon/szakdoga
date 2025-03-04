#pragma once 

#include <eigen3/Eigen/Core>
#include "grid.hpp"

template <int N>
using V = Eigen::Matrix<double, N, 1>;

template <int N>
using RV = Eigen::Matrix<double, 1, N>;

template <int R, int C>
using M = Eigen::Matrix<double, R, C>;

template <int S, int R, int C>
M<S * R, S * C> inflate(M<R, C> m)
{
    M<S * R, S * C> f;
    for (auto [i, j] : range2d{R, C}) {
        f.template block<S, S>(S * i, S * j) = m(i, j) * M<S, S>::Identity();
    }
    return f;
}

