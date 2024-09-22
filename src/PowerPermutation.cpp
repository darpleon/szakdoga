#include "PowerPermutation.hpp"
#include <iostream>

PowerPermutation::PowerPermutation(const std::unordered_map<Variable, unsigned int>& m) :
    monomials{m}
{
    discard_constants();
}

PowerPermutation::PowerPermutation(std::initializer_list<std::pair<const Variable, unsigned int>> init_list) :
    monomials{init_list}
{
    discard_constants();
}

PowerPermutation::PowerPermutation(Variable x) :
    monomials{{x, 1}}
{}

const std::unordered_map<Variable, unsigned int>& PowerPermutation::get_monomials() const
{
    return monomials;
}

PowerPermutation PowerPermutation::operator*(const PowerPermutation& pp) const
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

PowerPermutation PowerPermutation::operator*(Variable x) const
{
    return *this * PowerPermutation{x};
}

bool PowerPermutation::operator==(const PowerPermutation& pp) const
{
    if (this->monomials.size() != pp.monomials.size()) {
        return false;
    }
    for (const auto& [variable, power] : monomials) {
        if (!pp.monomials.contains(variable)) {
            return false;
        }
        if (pp.monomials.at(variable) != power) {
            return false;
        }
    }
    return true;
}

void PowerPermutation::discard_constants()
{
    std::erase_if(monomials, [](const auto& monomial)
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
    for (const auto& [variable, power] : pp.get_monomials()) {
        if (!first) {
            os << " ";
        }
        os << variable << "^" << power;
        first = false;
    }
    if (pp.get_monomials().empty()) {
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
    size_t result = pp.get_monomials().size();
    for (auto [variable, power] : pp.get_monomials()) {
        unsigned int v = hash_uint(variable.get_id());
        unsigned int p = hash_uint(power);
        result ^= v;
        result ^= p;
    }
    return result;
}
