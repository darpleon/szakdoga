#pragma once

#include "Polynomial.hpp"

template <typename coeff_type>
class Rational
{
public:
    Rational(Polynomial<coeff_type> num, Polynomial<coeff_type> den);

    template <typename input_type>
    using result_type = quotient_type<typename Polynomial<coeff_type>::template result_type<input_type>>;

    template <typename input_type>
    result_type<input_type> evaluate(std::initializer_list<std::pair<
        const Variable, const input_type&>> init_list) const;
private:
    Polynomial<coeff_type> numerator;
    Polynomial<coeff_type> denominator;
};

template <typename coeff_type>
Rational<coeff_type>::Rational(Polynomial<coeff_type> num, Polynomial<coeff_type> den) :
    numerator{num}, denominator{den}
{}

template <typename coeff_type>
template <typename input_type>
Rational<coeff_type>::result_type<input_type> Rational<coeff_type>::evaluate(
    std::initializer_list<std::pair<const Variable, const input_type&>> inputs) const
{
    return numerator.template evaluate<input_type>(inputs) / denominator.template evaluate<input_type>(inputs);
}
