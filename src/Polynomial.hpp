#pragma once

#include <iostream>
#include <map>
#include <ostream>
#include <unordered_map>

unsigned int hash_uint(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

class Variable
{
public:
    Variable() :
        id(id_counter++)
    {}

    unsigned int get_id() const
    {
        return id;
    }

    bool operator==(const Variable x) const
    {
        return this->id == x.id;
    }
private:
    static unsigned int id_counter;
    unsigned int id;
};

unsigned int Variable::id_counter = 0;

inline std::ostream& operator<<(std::ostream& os, Variable x)
{
    os << "$" << x.get_id();
    return os;
}

template<>
struct std::hash<Variable>
{
    size_t operator()(Variable x) const
    {
        cout << "hashin variable\n";
        return x.get_id();
    }
};

class PowerPermutation
{
public:
    PowerPermutation(const std::unordered_map<Variable, unsigned int>& m) :
        monomials{m}
    {}
    
    PowerPermutation(std::initializer_list<std::pair<const Variable, unsigned int>> init_list) :
        monomials{init_list}
    {}

    const std::unordered_map<Variable, unsigned int>& get_monomials() const
    {
        return monomials;
    }

    bool operator==(const PowerPermutation& p) const
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
private:
    std::unordered_map<Variable, unsigned int> monomials;
};

template<>
struct std::hash<PowerPermutation>
{
    size_t operator()(const PowerPermutation& pp) const
    {
        std::cout << "hashin\n";
        size_t seed = pp.get_monomials().size();
        for (auto [variable, power] : pp.get_monomials()) {
            unsigned int v = hash_uint(variable.get_id());
            unsigned int p = hash_uint(power);
            seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= p + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

template <typename T>
T zero();

template <typename T>
T one();

template <>
inline double zero() {return 0.0;}

template <>
inline double one() {return 1.0;}

inline std::ostream& operator<<(std::ostream& os, PowerPermutation p)
{
    for (const auto& [variable, power] : p.get_monomials()) {
        os << variable << "^" << power;
    }
    return os;
}

inline PowerPermutation operator^(const Variable& x, unsigned int k)
{
    return PowerPermutation{ {x, k} };
}

template <typename coeff_type = double>
class Polynomial
{
public:
    Polynomial(PowerPermutation pp)
    {
        terms[pp] = one<coeff_type>();
    }
private:
    std::unordered_map<PowerPermutation, coeff_type> terms;
};
