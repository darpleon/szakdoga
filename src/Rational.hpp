#pragma once

#include "Polynomial.hpp"
#include <format>

template <typename coeff_type>
class Rational
{
public:
    Rational();

    Rational(Polynomial<coeff_type> num, Polynomial<coeff_type> den);

    Rational(Polynomial<coeff_type> num);

    const Polynomial<coeff_type>& numerator() const;

    const Polynomial<coeff_type>& denominator() const;

    bool operator==(const Rational& r) const;

    void operator+=(const Rational& r);

    Rational operator+(const Rational& r) const;

    void operator*=(const Rational& r);

    Rational operator*(const Rational& r) const;

    void operator/=(const Rational& r);

    Rational operator/(const Rational& r) const;

    template <typename input_type>
    using result_type = quotient_type<
        typename Polynomial<coeff_type>::template result_type<input_type>>;

    template <typename input_type>
    result_type<input_type> evaluate(std::initializer_list<std::pair<
        const Variable, const input_type&>> init_list) const;

    Rational derivative(Variable x) const;
private:
    Polynomial<coeff_type> numerator_;
    Polynomial<coeff_type> denominator_;
};

template <typename coeff_type>
Rational<coeff_type> operator/(Polynomial<coeff_type> num, Polynomial<coeff_type> den);

template <typename coeff_type>
std::ostream& operator<<(std::ostream& os, Rational<coeff_type> r);

template <typename coeff_type>
Rational<coeff_type>::Rational() :
    numerator_{I<coeff_type>::zero}, denominator_{I<coeff_type>::one}
{}

template <typename coeff_type>
Rational<coeff_type>::Rational(Polynomial<coeff_type> num, Polynomial<coeff_type> den) :
    numerator_{num}, denominator_{den}
{}


template <typename coeff_type>
Rational<coeff_type>::Rational(Polynomial<coeff_type> num) :
    numerator_{num}, denominator_{I<coeff_type>::one}
{}

template <typename coeff_type>
const Polynomial<coeff_type>& Rational<coeff_type>::numerator() const
{
    return numerator_;
}

template <typename coeff_type>
const Polynomial<coeff_type>& Rational<coeff_type>::denominator() const
{
    return denominator_;
}

template <typename coeff_type>
bool Rational<coeff_type>::operator==(const Rational& r) const
{
    return (numerator_ == r.numerator_) && (denominator_ == r.denominator_);
}

template <typename coeff_type>
void Rational<coeff_type>::operator+=(const Rational& r)
{
    numerator_ = numerator_ * r.denominator_ + denominator_ * r.numerator_;
    denominator_ = denominator_ * r.denominator_;
}

template <typename coeff_type>
Rational<coeff_type> Rational<coeff_type>::operator+(const Rational& r) const
{
    return Rational{numerator_ * r.denominator_ + denominator_ * r.numerator_,
                    denominator_ * r.denominator_};
}

template <typename coeff_type>
void Rational<coeff_type>::operator*=(const Rational<coeff_type>& r)
{
    numerator_ *= r.numerator_;
    denominator_ *= r.denominator_;
}

template <typename coeff_type>
Rational<coeff_type> Rational<coeff_type>::operator*(const Rational<coeff_type>& r) const
{
    return Rational{numerator_ * r.numerator_, denominator_ * r.denominator_};
}

template <typename coeff_type>
void Rational<coeff_type>::operator/=(const Rational<coeff_type>& r)
{
    numerator_ *= r.denominator_;
    denominator_ *= r.numerator_;
}

template <typename coeff_type>
Rational<coeff_type> Rational<coeff_type>::operator/(const Rational<coeff_type>& r) const
{
    return Rational{numerator_ * r.denominator_, denominator_ * r.numerator_};
}

template <typename coeff_type>
template <typename input_type>
Rational<coeff_type>::result_type<input_type> Rational<coeff_type>::evaluate(
    std::initializer_list<std::pair<const Variable, const input_type&>> inputs) const
{
    return numerator_.template evaluate<input_type>(inputs) /
           denominator_.template evaluate<input_type>(inputs);
}

template <typename coeff_type>
Rational<coeff_type> Rational<coeff_type>::derivative(Variable x) const
{
    return Rational{numerator_.derivative(x) * denominator_ - numerator_ * denominator_.derivative(x),
                    denominator_ * denominator_};
}

template <typename coeff_type>
Rational<coeff_type> operator/(Polynomial<coeff_type> num, Polynomial<coeff_type> den)
{
    return Rational<coeff_type>(num, den);
}

template <typename coeff_type>
std::ostream& operator<<(std::ostream& os, Rational<coeff_type> r)
{
    os << "(" << r.numerator() << ") / (" << r.denominator() << ")";
    return os;
}
