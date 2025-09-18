#include <type_traits>
#include "digits.hpp"
template <typename T, typename U>
struct Sum {
    using Carry = std::conditional_t<std::is_same_v<U, One>, One, Zero>;
    using Remainder = T;
    void summer() {};
};

template <typename, typename = void>
struct is_sum : std::false_type {};

template <typename T, typename U>
struct is_sum<Sum<T, U>, std::void_t<decltype(std::declval<Sum<T, U>>().summer())>> : std::true_type {};

template <typename D1, typename D2>
struct AddDigits; // primary template

template <>
struct AddDigits<One, One> {
    using type = Sum<Two, Zero>;
};

template <>
struct AddDigits<One, Two> {
    using type = Sum<Three, Zero>;
};
template <>
struct AddDigits<One, Three> {
    using type = Sum<Four, Zero>;
};

template <>
struct AddDigits<One, Four> {
    using type = Sum<Five, Zero>;
};
template <>
struct AddDigits<One, Five> {
    using type = Sum<Six, Zero>;
};

template <>
struct AddDigits<One, Six> {
    using type = Sum<Seven, Zero>;
};
template <>
struct AddDigits<One, Seven> {
    using type = Sum<Eight, Zero>;
};

template <>
struct AddDigits<One, Eight> {
    using type = Sum<Nine, Zero>;
};

template <>
struct AddDigits<One, Nine> {
    using type = Sum<Zero, One>;
};