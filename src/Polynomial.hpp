#pragma once

#include "Variable.hpp"
#include "PowerPermutation.hpp"

#include <iostream>
#include <map>
#include <ostream>
#include <unordered_map>
#include <vector>

template <typename T>
T zero();

template <typename T>
T one();

template <>
inline double zero() {return 0.0;}

template <>
inline double one() {return 1.0;}

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
