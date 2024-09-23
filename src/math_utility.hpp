#include <utility>
template <typename T>
struct I
{
    static const T zero;
    static const T one;
};

template <typename T, typename U = T>
using sum_type = decltype(std::declval<T>() + std::declval<U>());

template <typename T, typename U = T>
using product_type = decltype(std::declval<T>() * std::declval<U>());

template <typename T, typename U = T>
using quotient_type = decltype(std::declval<T>() / std::declval<U>());
