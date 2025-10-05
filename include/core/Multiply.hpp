

#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"

template <typename D1, typename D2>
struct multiply_digit {
    static_assert(std::is_base_of_v<Digit, D1> && 
        std::is_base_of_v<Digit, D2>, "not a digit");
    using curr = MultiplyDigits<D1, D2>;
};

template <typename T, typename U, typename = void>
struct MultiplySingle {};

template <typename T, typename U, typename Carry>
struct MultiplySingle<mp_list<T>, mp_list<U>, Carry> {
    using curr = typename multiply_digit<T, U>::curr::type;
    using c = typename add_digit<Carry, typename curr::Remainder>::curr::type;
    using d = typename add_digit<typename c::Carry, typename curr::Carry>::curr::type;
    using rest = typename d::Remainder;
    using ans = std::conditional_t<std::is_same_v<Zero, rest>,
        mp_list<typename c::Remainder>, mp_list<typename c::Remainder, mp_list<rest>>>;
};

template <typename D1, typename... D1s, template <typename...> class Number1, 
          typename D2, template <typename...> class Number2, typename Carry>
struct MultiplySingle<Number1<D1, D1s...>, Number2<D2>, Carry> {
    using curr = typename multiply_digit<D1, D2>::curr::type; // i.e., Sum<Six, Four> type
    using c = typename add_digit<Carry, typename curr::Remainder>::curr::type;
    using d = typename add_digit<typename c::Carry, typename curr::Carry>::curr::type;
    using rest = MultiplySingle<Number1<D1s...>, Number2<D2>, typename d::Remainder>;
    using ans = mp_list<typename c::Remainder, typename rest::ans>;
};


template <typename T, typename U, size_t N = 0>
struct Multiply {};

template <typename... Ts, typename U, size_t N>
struct Multiply<mp_list<Ts...>, mp_list<U>, N> {
    using curr = typename flatten<typename MultiplySingle<mp_list<Ts...>, mp_list<U>, Zero>::ans>::res;
    using ans = mp_list<mp_append<mp_repeat_c<mp_list<Zero>, N>, curr>>;
};

template <typename... D1s, template <typename...> class Number1, 
          typename D2, typename... D2s, template <typename...> class Number2, size_t N>
struct Multiply<Number1<D1s...>, Number2<D2, D2s...>, N> {
    using curr = typename flatten<typename MultiplySingle<Number1<D1s...>, Number2<D2>, Zero>::ans>::res;
    using rest = Multiply<Number1<D1s...>, Number2<D2s...>, N + 1>;
    using ans = mp_append<mp_list<mp_append<mp_repeat_c<mp_list<Zero>, N>, curr>>, mp_flatten<mp_list<typename rest::ans>>>;
};

#endif