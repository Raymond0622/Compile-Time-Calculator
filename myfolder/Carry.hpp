#include "digits.hpp"
template <typename T>
struct Carried {};

template<>
struct Carried<Zero> {
    using D = One;
    using carry = Zero;
};

template<>
struct Carried<One> {
    using D = Two;
    using carry = Zero;
};

template<>
struct Carried<Two> {
    using D = Three;
    using carry = Zero;
};

template<>
struct Carried<Three> {
    using D = Four;
    using carry = Zero;
};

template<>
struct Carried<Four> {
    using D = Five;
    using carry = Zero;
};

template<>
struct Carried<Five> {
    using D = Six;
    using carry = Zero;
};

template<>
struct Carried<Six> {
    using D = Seven;
    using carry = Zero;
};

template<>
struct Carried<Seven> {
    using D = Eight;
    using carry = Zero;
};

template<>
struct Carried<Eight> {
    using D = Nine;
    using carry = Zero;
};

template<>
struct Carried<Nine> {
    using D = Zero;
    using carry = One;
};