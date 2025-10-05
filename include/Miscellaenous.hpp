#ifndef MISCELLAENOUS_HPP
#define MISCELLAENOUS_HPP

template <typename T, typename U, bool cond = false>
struct Swap {};

template <typename T, typename U>
struct Swap<T, U, true> {
    using num1 = T;
    using num2 = U;
};

template <typename T, typename U>
struct Swap<T, U, false> {
    using num1 = U;
    using num2 = T;
};

// Compare two numbers, if Number1 >= Number2, output is
// "ans" is std::true_type, else std::false_type

template <typename T, typename U>
struct Compare {};

template <typename D1, template <typename...> class Number1, 
        typename D2, template <typename...> class Number2> 
struct Compare<Number1<D1>, Number2<D2>> {
    using ans = std::conditional_t<D1::val >= D2::val, std::true_type, std::false_type>;
};

template <typename D1, typename... D1s, template <typename...> class Number1, 
        typename D2, typename... D2s, template <typename...> class Number2> 
struct Compare<Number1<D1, D1s...>, Number2<D2, D2s...>> {
    using c = std::conditional_t<D1::val == D2::val, std::true_type, std::false_type>;
    using ans = std::conditional_t<c::value, 
        typename Compare<Number1<D1s...>, Number2<D2s...>>::ans, 
        std::conditional_t<(D1::val > D2::val), std::true_type, std::false_type>>;
};

#endif
