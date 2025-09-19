#ifndef SUM_HPP
#define SUM_HPP
template <typename T, typename U>
struct Sum {
    using Carry = U;
    using Remainder = T;
    void summer() {};
};

template <typename, typename = void>
struct is_sum : std::false_type {};

template <typename T, typename U>
struct is_sum<Sum<T, U>, std::void_t<decltype(std::declval<Sum<T, U>>().summer())>> : std::true_type {};

template <typename D1, typename D2>
struct AddDigits {
    static constexpr int sum = D1::val + D2::val;
    static constexpr int rem = sum % 10;
    static constexpr int carry = sum / 10;

    using type = Sum<typename convertIntDigit<rem>::D, typename convertIntDigit<carry>::D>;
};

template <typename D1, typename D2>
struct MultiplyDigits {
    static constexpr int sum = D1::val * D2::val;
    static constexpr int rem = sum % 10;
    static constexpr int carry = sum / 10;

    using type = Sum<typename convertIntDigit<rem>::D, typename convertIntDigit<carry>::D>;
};

#endif
