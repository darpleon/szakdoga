#pragma once

#include <ostream>

class Variable
{
public:
    Variable();

    unsigned int id() const;

    bool operator==(const Variable x) const;
private:
    static unsigned int id_counter_;
    unsigned int id_;
};

std::ostream& operator<<(std::ostream& os, Variable x);

template<>
struct std::hash<Variable>
{
    size_t operator()(Variable x) const;
};
