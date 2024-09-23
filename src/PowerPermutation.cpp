#include "PowerPermutation.hpp"
#include <iostream>

PowerPermutation::PowerPermutation(const std::unordered_map<Variable, unsigned int>& m) :
    monomials_{m}
{
    discard_constants();
}

PowerPermutation::PowerPermutation(std::initializer_list<std::pair<const Variable, unsigned int>> init_list) :
    monomials_{init_list}
{
    discard_constants();
}

PowerPermutation::PowerPermutation(Variable x) :
    monomials_{{x, 1}}
{}

const std::unordered_map<Variable, unsigned int>& PowerPermutation::monomials() const
{
    return monomials_;
}

void PowerPermutation::operator*=(std::pair<const Variable, unsigned int> monomial)
{
    auto [variable, power] = monomial;
    if (monomials_.contains(variable)) {
        monomials_[variable] += power;
    }
    else {
        monomials_[variable] = power;
    }
}

void PowerPermutation::operator*=(const PowerPermutation& pp)
{
    for (const std::pair<const Variable, unsigned int>& monomial: pp.monomials_) {
        *this *= monomial;
    }
}

PowerPermutation PowerPermutation::operator*(const PowerPermutation& pp) const
{
    PowerPermutation result{*this};
    result *= pp;
    return result;
}

void PowerPermutation::operator*=(Variable x)
{
    *this *= {x, 1};
}

PowerPermutation PowerPermutation::operator*(Variable x) const
{
    PowerPermutation result{*this};
    result *= x;
    return result;
}

bool PowerPermutation::operator==(const PowerPermutation& pp) const
{
    if (this->monomials_.size() != pp.monomials_.size()) {
        return false;
    }
    for (const auto& [variable, power] : monomials_) {
        if (!pp.monomials_.contains(variable)) {
            return false;
        }
        if (pp.monomials_.at(variable) != power) {
            return false;
        }
    }
    return true;
}

void PowerPermutation::discard_constants()
{
    std::erase_if(monomials_, [](const auto& monomial)
    {
        const auto& [variable, power] = monomial;
        return power == 0;
    });
}

PowerPermutation operator^(Variable x, unsigned int k)
{
    return PowerPermutation{{x, k}};
}

PowerPermutation operator*(Variable x, Variable y)
{
    if (x == y) {
        return x^2;
    }
    return PowerPermutation {{x, 1}, {y, 1}};
}

PowerPermutation operator*(Variable x, PowerPermutation pp)
{
    return pp * x;
}

std::ostream& operator<<(std::ostream& os, PowerPermutation pp)
{
    bool first = true;
    os << "(";
    for (const auto& [variable, power] : pp.monomials()) {
        if (!first) {
            os << " ";
        }
        os << variable << "^" << power;
        first = false;
    }
    if (pp.monomials().empty()) {
        os << "1";
    }
    os << ")";
    return os;
}

unsigned int hash_uint(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

size_t std::hash<PowerPermutation>::operator()(const PowerPermutation& pp) const
{
    size_t result = pp.monomials().size();
    for (auto [variable, power] : pp.monomials()) {
        unsigned int v = hash_uint(variable.id());
        unsigned int p = hash_uint(power);
        result ^= v;
        result ^= p;
    }
    return result;
}
