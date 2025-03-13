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
    typedef std::pair<const PowerPermutation, coeff_type> term_type;

    Polynomial();

    Polynomial(const coeff_type& constant);

    Polynomial(Variable x);

    Polynomial(const PowerPermutation& pp);

    Polynomial(const std::unordered_map<PowerPermutation, coeff_type>& terms);

    Polynomial(std::initializer_list<term_type> init_list);


    const std::unordered_map<PowerPermutation, coeff_type>& terms() const;


    bool operator==(const Polynomial& p) const;


    void operator+=(const Polynomial& p);

    void operator+=(const term_type& term);

    void operator+=(const PowerPermutation& pp);

    void operator+=(coeff_type constant);


    Polynomial operator+(const Polynomial& p) const;

    Polynomial operator+(const PowerPermutation& pp) const;

    Polynomial operator+(Variable x) const;

    Polynomial operator+(const coeff_type& constant) const;


    Polynomial operator-() const;

    Polynomial operator-(const Polynomial& p) const;

    Polynomial operator-(const PowerPermutation& pp) const;


    void operator*=(const Polynomial& p);


    Polynomial operator*(const Polynomial& p) const;

    Polynomial operator*(const PowerPermutation& pp) const;

    Polynomial operator*(Variable x) const;

    Polynomial operator*(const coeff_type& multiplier) const;


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

    std::unordered_map<PowerPermutation, coeff_type> terms_;


    void discard_zeros();
};


template <typename coeff_type>
Polynomial<coeff_type> operator+(const PowerPermutation& pp, const Polynomial<coeff_type>& p);

template <typename coeff_type>
Polynomial<coeff_type> operator+(Variable x, const Polynomial<coeff_type>& p);

template <typename coeff_type>
Polynomial<coeff_type> operator+(const coeff_type& constant, const Polynomial<coeff_type>& p);


template <typename coeff_type>
Polynomial<coeff_type> operator*(const PowerPermutation& pp, const Polynomial<coeff_type>& p);

template <typename coeff_type>
Polynomial<coeff_type> operator*(Variable x, const Polynomial<coeff_type>& p);

template <typename coeff_type>
Polynomial<coeff_type> operator*(const coeff_type& multiplier, const Polynomial<coeff_type>& p);


template <typename coeff_type>
Polynomial<coeff_type> operator+(const PowerPermutation& pp, coeff_type coeff);
template <typename coeff_type>
Polynomial<coeff_type> operator+(coeff_type coeff, const PowerPermutation& pp);

template <typename coeff_type>
Polynomial<coeff_type> operator+(Variable x, coeff_type coeff);
template <typename coeff_type>
Polynomial<coeff_type> operator+(coeff_type coeff, Variable x);


template <typename coeff_type>
Polynomial<coeff_type> operator-(const PowerPermutation& pp, coeff_type coeff);
template <typename coeff_type>
Polynomial<coeff_type> operator-(coeff_type coeff, const PowerPermutation& pp);

template <typename coeff_type>
Polynomial<coeff_type> operator-(Variable x, coeff_type coeff);
template <typename coeff_type>
Polynomial<coeff_type> operator-(coeff_type coeff, Variable x);


template <typename coeff_type>
Polynomial<coeff_type> operator*(const PowerPermutation& pp, coeff_type coeff);
template <typename coeff_type>
Polynomial<coeff_type> operator*(coeff_type coeff, const PowerPermutation& pp);

template <typename coeff_type>
Polynomial<coeff_type> operator*(Variable x, coeff_type coeff);
template <typename coeff_type>
Polynomial<coeff_type> operator*(coeff_type coeff, Variable x);


template <typename coeff_type>
std::ostream& operator<<(std::ostream& os, Polynomial<coeff_type> p);


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
Polynomial<coeff_type>::Polynomial()
{}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(const coeff_type& constant) :
    terms_{{PowerPermutation{}, constant}}
{
    discard_zeros();
}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(Variable x) :
    terms_{{{{x, 1}}, I<coeff_type>::one}}
{}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(const PowerPermutation& pp) :
    terms_{{pp, I<coeff_type>::one}}
{}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(const std::unordered_map<PowerPermutation, coeff_type>& terms) :
    terms_{terms}
{
    discard_zeros();
}

template <typename coeff_type>
Polynomial<coeff_type>::Polynomial(std::initializer_list<Polynomial<coeff_type>::term_type> init_list) :
    terms_{init_list}
{
    discard_zeros();
}


template <typename coeff_type>
const std::unordered_map<PowerPermutation, coeff_type>& Polynomial<coeff_type>::terms() const
{
    return terms_;
}


template <typename coeff_type>
bool Polynomial<coeff_type>::operator==(const Polynomial& p) const
{
    if (this->terms_.size() != p.terms_.size()) {
        return false;
    }
    for (const auto& [permutation, coeff] : terms_) {
        if (!p.terms_.contains(permutation)) {
            return false;
        }
        if (p.terms_.at(permutation) != coeff) {
            return false;
        }
    }
    return true;
}


template <typename coeff_type>
void Polynomial<coeff_type>::operator+=(const Polynomial& p)
{
    for (const term_type& term : p.terms_) {
        *this += term;
    }
}

template <typename coeff_type>
void Polynomial<coeff_type>::operator+=(const Polynomial<coeff_type>::term_type& term)
{
    const auto& [permutation, coeff] = term;
    if (terms_.contains(permutation)) {
        terms_[permutation] += coeff;
    }
    else {
        terms_[permutation] = coeff;
    }
    discard_zeros();
}

template <typename coeff_type>
void Polynomial<coeff_type>::operator+=(const PowerPermutation& pp)
{
    *this += {pp, I<coeff_type>::one};
}

template <typename coeff_type>
void Polynomial<coeff_type>::operator+=(coeff_type constant)
{
    *this += {{}, constant};
}


template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator+(const Polynomial& p) const
{
    Polynomial result{*this};
    result += p;
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator+(const PowerPermutation& pp) const
{
    Polynomial result{*this};
    result += pp;
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator+(Variable x) const
{
    Polynomial result{*this};
    result += {{{x, 1}}, I<coeff_type>::one};
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator+(const coeff_type& constant) const
{
    Polynomial result{*this};
    result += {{}, constant};
    return result;
}


template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator-() const
{
    Polynomial result{};
    for (const auto& [permutation, coeff] : terms_) {
        result.terms_[permutation] = -coeff;
    }
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator-(const Polynomial& p) const
{
    return *this + -p;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator-(const PowerPermutation& pp) const
{
    return *this - Polynomial{pp};
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
    for (const auto& [pp_left, c_left] : terms_) {
        for (const auto& [pp_right, c_right] : p.terms_) {
            PowerPermutation pp_product = pp_left * pp_right;
            coeff_type c_product = c_left * c_right;
            result += {pp_product, c_product};
        }
    }
    return result;
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator*(const PowerPermutation& pp) const
{
    return *this * Polynomial{pp};
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator*(Variable x) const
{
    return *this * Polynomial{x};
}

template <typename coeff_type>
Polynomial<coeff_type> Polynomial<coeff_type>::operator*(const coeff_type& multiplier) const
{
    Polynomial result{*this};
    for (auto& [permutation, coeff] : result.terms_) {
        coeff *= multiplier;
    }
    return result;
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
    for (const auto& [permutation, coeff] : terms_) {
        product_type<input_type> product = I<product_type<input_type>>::one;
        for (auto [variable, power] : permutation.monomials()) {
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
Polynomial<coeff_type> Polynomial<coeff_type>::derivative(Variable x) const
{
    Polynomial result{};
    for (const auto& [permutation, coeff] : terms_) {
        if (permutation.monomials().contains(x)) {
            std::unordered_map<Variable, unsigned int> monomials = permutation.monomials();
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
void Polynomial<coeff_type>::discard_zeros()
{
    std::erase_if(terms_, [](const auto& term)
    {
        const auto& [permutation, coeff] = term;
        coeff_type tolerance = std::numeric_limits<coeff_type>::epsilon();
        return (coeff >= -tolerance) && (coeff <= tolerance);
    });
}


template <typename coeff_type>
Polynomial<coeff_type> operator+(const PowerPermutation& pp, const Polynomial<coeff_type>& p)
{
    return p + pp;
}
template <typename coeff_type>
Polynomial<coeff_type> operator+(Variable x, const Polynomial<coeff_type>& p)
{
    return p + x;
}
template <typename coeff_type>
Polynomial<coeff_type> operator+(const coeff_type& constant, const Polynomial<coeff_type>& p)
{
    return p + constant;
}


template <typename coeff_type>
Polynomial<coeff_type> operator*(const PowerPermutation& pp, const Polynomial<coeff_type>& p)
{
    return p * pp;
}
template <typename coeff_type>
Polynomial<coeff_type> operator*(Variable x, const Polynomial<coeff_type>& p)
{
    return p * x;
}
template <typename coeff_type>
Polynomial<coeff_type> operator*(const coeff_type& multiplier, const Polynomial<coeff_type>& p)
{
    return p * multiplier;
}


template <typename coeff_type>
Polynomial<coeff_type> operator+(const PowerPermutation& pp, coeff_type coeff)
{
    return Polynomial<coeff_type>{{pp, I<coeff_type>::one}, {{}, coeff}};
}
template <typename coeff_type>
Polynomial<coeff_type> operator+(coeff_type coeff, const PowerPermutation& pp)
{
    return pp + coeff;
}

template <typename coeff_type>
Polynomial<coeff_type> operator+(Variable x, coeff_type coeff)
{
    return Polynomial{{{{x, 1}}, I<coeff_type>::one}, {{}, coeff}};
}
template <typename coeff_type>
Polynomial<coeff_type> operator+(coeff_type coeff, Variable x)
{
    return x + coeff;
}


template <typename coeff_type>
Polynomial<coeff_type> operator-(const PowerPermutation& pp, coeff_type coeff)
{
    return Polynomial<coeff_type>{{pp, I<coeff_type>::one}, {{}, -coeff}};
}
template <typename coeff_type>
Polynomial<coeff_type> operator-(coeff_type coeff, const PowerPermutation& pp)
{
    return -(pp - coeff);
}

template <typename coeff_type>
Polynomial<coeff_type> operator-(Variable x, coeff_type coeff)
{
    return Polynomial{{{{x, 1}}, I<coeff_type>::one}, {{}, -coeff}};
}
template <typename coeff_type>
Polynomial<coeff_type> operator-(coeff_type coeff, Variable x)
{
    return -(x - coeff);
}


template <typename coeff_type>
Polynomial<coeff_type> operator-(coeff_type coeff, const Polynomial<coeff_type>& p)
{
    return -(p - coeff);
}


template <typename coeff_type>
Polynomial<coeff_type> operator*(const PowerPermutation& pp, coeff_type coeff)
{
    return Polynomial<coeff_type>{{pp, coeff}};
}
template <typename coeff_type>
Polynomial<coeff_type> operator*(coeff_type coeff, const PowerPermutation& pp)
{
    return pp * coeff;
}

template <typename coeff_type>
Polynomial<coeff_type> operator*(Variable x, coeff_type coeff)
{
    return Polynomial<coeff_type>{{{{x, 1}}, coeff}};
}
template <typename coeff_type>
Polynomial<coeff_type> operator*(coeff_type coeff, Variable x)
{
    return x * coeff;
}


template <typename coeff_type>
std::ostream& operator<<(std::ostream& os, Polynomial<coeff_type> p)
{
    bool first = true;
    for (const auto& [pp, coeff] : p.terms()) {
        if (!first) {
            os << " + ";
        }
        os << pp << "*" << coeff;
        first = false;
    }
    return os;
}
