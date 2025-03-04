#pragma once 

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <array>
#include <vector>

template<typename T>
class grid
{
public:
    grid(size_t n, size_t m)
        : _n{n}, _m{m}, _d(n * m)
    {
    }

    grid(grid&& g) : _n{g._n}, _m{g._m}, _d{std::move(g._d)} {}

    size_t n() const
    {
        return _n;
    }

    size_t m() const
    {
        return _m;
    }

    grid(size_t n, size_t m, std::initializer_list<T> contents)
        : _n(n), _m(m), _d(n * m)
    {
        std::ranges::copy(contents.begin(), contents.end(), _d.begin());
    }

    const T& operator[](size_t i, size_t j) const
    {
        return _d[i * _n + j];
    }

    T& operator[](size_t i, size_t j)
    {
        return _d[i * _n + j];
    }

    using iterator = std::vector<T>::iterator;
    using const_iterator = std::vector<T>::const_iterator;

    iterator begin() { return _d.begin(); }
    iterator end() { return _d.end(); }
    const_iterator begin() const { return _d.begin(); }
    const_iterator end() const { return _d.end(); }
    const_iterator cbegin() const { return _d.cbegin(); }
    const_iterator cend() const { return _d.cend(); }

private:
    const size_t _n, _m;
    std::vector<T> _d;

};

struct idx2d
{
    idx2d(size_t m, size_t i, size_t j) :m(m), idx{i, j} {}

    std::array<size_t, 2> operator*()
    {
        return idx;
    }

    void operator++()
    {
        if (idx[1] == m - 1) {
            ++idx[0];
            idx[1] = 0;
        }
        else {
            ++idx[1];
        }
    }

    bool operator!=(const idx2d& rhs) const
    {
        return idx != rhs.idx;
    }
private:
    std::array<size_t, 2> idx;
    size_t m;
};

class range2d
{
public:
    range2d(size_t n, size_t m) :n(n), m(m) {}

    idx2d begin()
    {
        return idx2d {m, 0, 0};
    }

    idx2d end()
    {
        return idx2d {m, n, 0};
    }

private:
    size_t n, m;
};


template <typename T>
range2d indices(const grid<T>& g)
{
    return range2d(g.n(), g.m());
}
