#pragma once 

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <array>
#include <vector>

class range2d;

template<typename T>
class grid
{
public:
    grid(size_t n, size_t m)
        : _size{n, m}, _d(n * m)
    {
    }

    grid(std::array<size_t, 2> size)
        : _size{size}, _d(size[0] * size[1])
    {
    }

    grid(grid&& g) : _size{g._size}, _d{std::move(g._d)} {}

    std::array<size_t, 2> size() const
    {
        return _size;
    }

    size_t n() const
    {
        return _size[0];
    }

    size_t m() const
    {
        return _size[1];
    }

    grid(size_t n, size_t m, std::initializer_list<T> contents)
        : _size{n, m}, _d(n * m)
    {
        std::ranges::copy(contents.begin(), contents.end(), _d.begin());
    }

    const T& operator[](size_t i, size_t j) const
    {
        return _d[i * _size[0] + j];
    }

    T& operator[](size_t i, size_t j)
    {
        return _d[i * _size[0] + j];
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
    const std::array<size_t, 2> _size;
    std::vector<T> _d;

};

struct idx2d
{
    idx2d(size_t m, size_t i, size_t j) :m(m), idx{i, j} {}

    idx2d(size_t m, std::array<size_t, 2> a) :m(m), idx{a} {}

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

    idx2d operator+(std::array<size_t, 2> a)
    {
        return idx2d(m, idx[0] + a[0], idx[1] + a[1]);
    }
    
    idx2d operator-(std::array<size_t, 2> a)
    {
        return idx2d(m, idx[0] - a[0], idx[1] - a[1]);
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

    range2d operator+(std::array<size_t, 2> a)
    {
        return range2d{n + a[0], m + a[1]};
    }

    range2d operator-(std::array<size_t, 2> a)
    {
        return range2d{n - a[0], m - a[1]};
    }

private:
    size_t n, m;
};


template <typename T>
range2d range(const grid<T>& g)
{
    return range2d(g.n(), g.m());
}
