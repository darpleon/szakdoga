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

    const std::unordered_map<Variable, unsigned int>& get_monomials() const;

    PowerPermutation operator*(const PowerPermutation& pp) const;

    bool operator==(const PowerPermutation& p) const;
private:
    std::unordered_map<Variable, unsigned int> monomials;
};

template<>
struct std::hash<PowerPermutation>
{
    size_t operator()(const PowerPermutation& pp) const;
};

std::ostream& operator<<(std::ostream& os, PowerPermutation p);

PowerPermutation operator^(const Variable& x, unsigned int k);
