#pragma once

#include "math_utility.hpp"
#include "Variable.hpp"
#include "PowerPermutation.hpp"

#include <ostream>
#include <unordered_map>
#include <vector>


template <typename coeff_type = double>
class Polynomial
{
public:
    Polynomial(const coeff_type& constant);

    Polynomial(const PowerPermutation& pp);

    Polynomial(const std::unordered_map<PowerPermutation, coeff_type>& terms);

    Polynomial(std::initializer_list<std::pair<
        const PowerPermutation, coeff_type>> init_list);

    const std::unordered_map<PowerPermutation, coeff_type>& get_terms() const;

    bool operator==(const Polynomial& p) const;

    Polynomial operator*(const coeff_type& multiplier);

    Polynomial operator+(const coeff_type& constant);

    void operator+=(std::pair<const PowerPermutation&, coeff_type> term);

    void operator+=(const Polynomial& p);

    Polynomial operator+(const Polynomial& p) const;

    void operator*=(const Polynomial& p);

    Polynomial operator*(const Polynomial& p) const;

    Polynomial operator-() const;

    template <typename input_type>
    using result_type = sum_type<product_type<
                                     product_type<input_type>,
                                     coeff_type>>;
                            

    template <typename input_type = double>
    result_type<input_type> evaluate(std::initializer_list<std::pair<
        const Variable, const input_type&>> init_list) const;

    template <typename input_type = double>
    result_type<input_type> operator()(std::initializer_list<std::pair<
        const Variable, const input_type&>> init_list) const;

    Polynomial derivative(Variable x) const;

    std::vector<Polynomial> derivative(std::initializer_list<Variable> variables) const;
private:

    std::unordered_map<PowerPermutation, coeff_type> terms;

    void discard_zeros();
};

template <typename coeff_type>
std::ostream& operator<<(std::ostream& os, Polynomial<coeff_type> p);

template <typename coeff_type>
Polynomial<coeff_type> operator*(const PowerPermutation& pp, coeff_type coeff);

template <typename coeff_type>
struct I<Polynomial<coeff_type>>
{
    static const Polynomial<coeff_type> zero;
    static const Polynomial<coeff_type> one;
};

template <typename coeff_type>
const Polynomial<coeff_type> I<Polynomial<coeff_type>>::zero = Polynomial<coeff_type>{};

template <typename coeff_type>
const Polynomial<coeff_type> I<Polynomial<coeff_type>>::one = Polynomial<coeff_type>{I<coeff_type>::one};

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(const coeff_type& constant) :
    terms{{PowerPermutation{}, constant}}
{
    discard_zeros();
}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(const PowerPermutation& pp) :
    terms{{pp, I<coeff_type>::one}}
{}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(const std::unordered_map<PowerPermutation, coeff_type>& terms) :
    terms{terms}
{
    discard_zeros();
}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(std::initializer_list<std::pair<
    const PowerPermutation, coeff_type>> init_list) :
    terms{init_list}
{
    discard_zeros();
}

template <typename coeff_type>
const std::unordered_map<PowerPermutation, coeff_type>& Polynomial<coeff_type>::get_terms() const
{
    return terms;
}

template <typename coeff_type>
bool Polynomial<coeff_type>::operator==(const Polynomial& p) const
{
    if (this->terms.size() != p.terms.size()) {
        return false;
    }
    for (const auto& [permutation, coeff] : terms) {
        if (!p.terms.contains(permutation)) {
            return false;
        }
        if (p.terms.at(permutation) != coeff) {
            return false;
        }
    }
    return true;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator*(const coeff_type& multiplier)
{
    Polynomial result{*this};
    for (auto& [permutation, coeff] : result.terms) {
        coeff *= multiplier;
    }
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator+(const coeff_type& constant)
{
    Polynomial result{*this};
    result += {{}, constant};
    return result;
}

template <typename coeff_type>
void Polynomial<coeff_type>::operator+=(std::pair<const PowerPermutation&, coeff_type> term)
{
    const auto& [permutation, coeff] = term;
    if (terms.contains(permutation)) {
        terms[permutation] += coeff;
    }
    else {
        terms[permutation] = coeff;
    }
}

template <typename coeff_type>
void Polynomial<coeff_type>::operator+=(const Polynomial& p)
{
    for (const std::pair<PowerPermutation, coeff_type>& term : p.terms) {
        *this += term;
    }
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator+(const Polynomial& p) const
{
    Polynomial result{*this};
    result += p;
    return result;
}

template <typename coeff_type>
void Polynomial<coeff_type>::operator*=(const Polynomial<coeff_type>& p)
{
    *this = *this * p;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator*(const Polynomial<coeff_type>& p) const
{
    Polynomial result{};
    for (const auto& [pp_left, c_left] : terms) {
        for (const auto& [pp_right, c_right] : p.terms) {
            PowerPermutation pp_product = pp_left * pp_right;
            coeff_type c_product = c_left * c_right;
            result += {pp_product, c_product};
        }
    }
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator-() const
{
    Polynomial result;
    for (const auto& [permutation, coeff] : terms) {
        result.terms[permutation] = -coeff;
    }
}

template <typename coeff_type>
template <typename input_type>
Polynomial<coeff_type>::result_type<input_type> Polynomial<coeff_type>::evaluate(
    std::initializer_list<std::pair<const Variable, const input_type&>> inputs) const
{
    std::unordered_map<Variable, std::vector<product_type<input_type>>> values;
    for (const auto& [variable, value] : inputs) {
        values[variable].push_back(I<product_type<input_type>>::one);
        values[variable].push_back(value);
    }
    result_type<input_type> result = I<result_type<input_type>>::zero;
    for (const auto& [permutation, coeff] : terms) {
        product_type<input_type> product = I<product_type<input_type>>::one;
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

template <typename coeff_type>
template <typename input_type>
Polynomial<coeff_type>::result_type<input_type> Polynomial<coeff_type>::operator()(
    std::initializer_list<std::pair<const Variable, const input_type&>> inputs) const
{
    return evaluate<input_type>(inputs);
}

template <typename coeff_type>
void Polynomial<coeff_type>::discard_zeros()
{
    std::erase_if(terms, [](const auto& term)
    {
        const auto& [permutation, coeff] = term;
        return coeff == I<coeff_type>::zero;
    });
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::derivative(Variable x) const
{
    Polynomial result{};
    for (const auto& [permutation, coeff] : terms) {
        if (permutation.get_monomials().contains(x)) {
            std::unordered_map<Variable, unsigned int> monomials = permutation.get_monomials();
            coeff_type c_result = coeff * monomials[x];
            monomials[x] -= 1;
            result += {{monomials}, c_result};
        }
    }
    return result;
}

template <typename coeff_type>
std::vector<Polynomial<coeff_type>> Polynomial<coeff_type>::derivative(
    std::initializer_list<Variable> variables) const
{
    std::vector<Polynomial> result{};
    for (Variable x : variables) {
        result.push_back(derivative(x));
    }
    return result;
}

template <typename coeff_type>
std::ostream& operator<<(std::ostream& os, Polynomial<coeff_type> p)
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
