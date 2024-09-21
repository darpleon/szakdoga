#pragma once

#include "Variable.hpp"
#include <unordered_map>

class PowerPermutation
{
public:
    PowerPermutation();

    PowerPermutation(const PowerPermutation& pp);

    PowerPermutation(const std::unordered_map<Variable, unsigned int>& m);

    PowerPermutation(std::initializer_list<std::pair<const Variable, unsigned int>> init_list);

    PowerPermutation(Variable x);
    
    const std::unordered_map<Variable, unsigned int>& get_monomials() const;

    PowerPermutation operator*(const PowerPermutation& pp) const;

    PowerPermutation operator*(Variable x) const;

    bool operator==(const PowerPermutation& pp) const;
private:
    std::unordered_map<Variable, unsigned int> monomials;

    void discard_constants();
};

PowerPermutation operator^(Variable x, unsigned int k);

PowerPermutation operator*(Variable x, Variable y);

PowerPermutation operator*(Variable x, PowerPermutation pp);

std::ostream& operator<<(std::ostream& os, PowerPermutation pp);

template<>
struct std::hash<PowerPermutation>
{
    size_t operator()(const PowerPermutation& pp) const;
};
