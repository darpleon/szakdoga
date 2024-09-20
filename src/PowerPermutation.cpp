#include "PowerPermutation.hpp"

PowerPermutation::PowerPermutation()
{}

PowerPermutation::PowerPermutation(const PowerPermutation& pp) :
    monomials{pp.monomials}
{}

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

bool PowerPermutation::operator==(const PowerPermutation& p) const
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

void PowerPermutation::discard_constants()
{
    std::erase_if(monomials, [](const auto& monomial)
    {
        const auto& [variable, power] = monomial;
        return power == 0;
    });
}

PowerPermutation operator^(const Variable& x, unsigned int k)
{
    return PowerPermutation{ {x, k} };
}

std::ostream& operator<<(std::ostream& os, PowerPermutation p)
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
    if (p.get_monomials().empty()) {
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
        result ^= v + 0x9e3779b9 + (result << 6) + (result >> 2);
        result ^= p + 0x9e3779b9 + (result << 6) + (result >> 2);
    }
    return result;
}
