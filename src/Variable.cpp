#include "Variable.hpp"

unsigned int Variable::id_counter_ = 0;

Variable::Variable() :
    id_(id_counter_++)
{}

unsigned int Variable::id() const
{
    return id_;
}

bool Variable::operator==(const Variable x) const
{
    return this->id_ == x.id_;
}

std::ostream& operator<<(std::ostream& os, Variable x)
{
    os << "$" << x.id();
    return os;
}

size_t std::hash<Variable>::operator()(Variable x) const
{
    return x.id();
}
