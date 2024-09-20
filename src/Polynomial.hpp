#pragma once

#include "Variable.hpp"

#include <iostream>
#include <map>
#include <ostream>
#include <unordered_map>
#include <vector>

unsigned int hash_uint(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

class PowerPermutation
{
public:
    PowerPermutation()
    {}

    PowerPermutation(const PowerPermutation& pp) :
        monomials{pp.monomials}
    {}

    PowerPermutation(const std::unordered_map<Variable, unsigned int>& m) :
        monomials{m}
    {}
    
    PowerPermutation(std::initializer_list<std::pair<const Variable, unsigned int>> init_list) :
        monomials{init_list}
    {}

    const std::unordered_map<Variable, unsigned int>& get_monomials() const
    {
        return monomials;
    }

    PowerPermutation operator*(const PowerPermutation& pp) const
    {
        PowerPermutation result{*this};
        for (const auto& [variable, power] : pp.monomials) {
            if (monomials.contains(variable)) {
                result.monomials[variable] += power;
            }
            else {
                result.monomials[variable] = power;
            }
        }
        return result;
    }

    bool operator==(const PowerPermutation& p) const
    {
        if (this->monomials.size() != p.monomials.size()) {
            return false;
        }
        for (const auto& [variable, power] : monomials) {
            if (!p.monomials.contains(variable)) {
                return false;
            }
            if (p.monomials.at(variable) != power) {
                return false;
            }
        }
        return true;
    }
private:
    std::unordered_map<Variable, unsigned int> monomials;
};

template<>
struct std::hash<PowerPermutation>
{
    size_t operator()(const PowerPermutation& pp) const
    {
        size_t seed = pp.get_monomials().size();
        for (auto [variable, power] : pp.get_monomials()) {
            unsigned int v = hash_uint(variable.get_id());
            unsigned int p = hash_uint(power);
            seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= p + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

template <typename T>
T zero();

template <typename T>
T one();

template <>
inline double zero() {return 0.0;}

template <>
inline double one() {return 1.0;}

inline std::ostream& operator<<(std::ostream& os, PowerPermutation p)
{
    bool first = true;
    os << "(";
    for (const auto& [variable, power] : p.get_monomials()) {
        if (!first) {
            os << " ";
        }
        os << variable << "^" << power;
        first = false;
    }
    os << ")";
    return os;
}

inline PowerPermutation operator^(const Variable& x, unsigned int k)
{
    return PowerPermutation{ {x, k} };
}

template <typename coeff_type = double>
class Polynomial
{
public:
    Polynomial()
    {}

    Polynomial(PowerPermutation pp)
    {
        terms[pp] = one<coeff_type>();
    }

    Polynomial(std::initializer_list<std::pair<const PowerPermutation, coeff_type>> init_list) :
        terms{init_list}
    {}

    const std::unordered_map<PowerPermutation, coeff_type>& get_terms() const
    {
        return terms;
    }

    Polynomial operator+(const Polynomial& p) const
    {
        Polynomial result{*this};
        for (const auto& [permutation, coeff] : p.get_terms()) {
            if (terms.contains(permutation)) {
                result.terms[permutation] += coeff;
            }
            else {
                result.terms[permutation] = coeff;
            }
        }
        return result;
    }

    Polynomial operator-() const
    {
        Polynomial result;
        for (const auto& [permutation, coeff] : terms) {
            result.terms[permutation] = -coeff;
        }
    }

    template <typename input_type = double>
    input_type operator()(std::initializer_list<std::pair<const Variable, input_type>> init_list) {
        std::unordered_map<Variable, std::vector<input_type>> values;
        for (auto [variable, value] : init_list) {
            values[variable].push_back(one<input_type>());
            values[variable].push_back(value);
        }
        input_type result = zero<input_type>();
        for (const auto& [permutation, coeff] : terms) {
            input_type product = one<input_type>();
            for (auto [variable, power] : permutation.get_monomials()) {
                while (power >= values[variable].size()) {
                    values[variable].push_back(values[variable].back() * values[variable][1]);
                }
                product *= values[variable][power];
            }
            result += product * coeff;
        }
        return result;
    }

private:
    std::unordered_map<PowerPermutation, coeff_type> terms;
};

template <typename coeff_type>
inline std::ostream& operator<<(std::ostream& os, Polynomial<coeff_type> p)
{
    bool first = true;
    for (const auto& [pp, coeff] : p.get_terms()) {
        if (!first) {
            os << " + ";
        }
        os << pp << "*" << coeff;
        first = false;
    }
    return os;
}

template <typename coeff_type>
Polynomial<coeff_type> operator*(const PowerPermutation& pp, coeff_type coeff)
{
    return Polynomial<coeff_type>{{pp, coeff}};
}
