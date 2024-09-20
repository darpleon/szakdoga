#include "Variable.hpp"

unsigned int Variable::id_counter = 0;

Variable::Variable() :
    id(id_counter++)
{}

unsigned int Variable::get_id() const
{
    return id;
}

bool Variable::operator==(const Variable x) const
{
    return this->id == x.id;
}

size_t std::hash<Variable>::operator()(Variable x) const
{
    return x.get_id();
}
