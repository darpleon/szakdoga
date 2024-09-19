#pragma once

#include <map>
#include <ostream>

unsigned int hash(unsigned int x) {
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

    bool operator<(const Variable x) const
    {
        return this->id < x.id;
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

class PowerPermutation
{
public:
    PowerPermutation(const std::map<Variable, unsigned int>& m) :
        monomials{m}
    {}
    
    PowerPermutation(std::initializer_list<std::pair<const Variable, unsigned int>> init_list) :
        monomials{init_list}
    {}

    const std::map<Variable, unsigned int>& get_monomials() const
    {
        return monomials;
    }

    // a < b if a has fewer variables
    //
    bool operator<(const PowerPermutation& p) const
    {
        if (this->monomials.size() < p.monomials.size()) {
            return true;
        }

        for (const auto& [variable, power] : monomials) {

        }
        return false;
    }
private:
    std::map<Variable, unsigned int> monomials;
};

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

template <typename coeff_type>
class PolynomialTerm
{
private:
    // std::map<
};
