#ifndef TYPELIST_HPP
#define TYPELIST_HPP

#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"

template <typename T>
struct is_mp_list : std::false_type {};

template <typename... Ts>
struct is_mp_list<mp_list<Ts...>> : std::true_type {};

template <typename T>
struct flatten {};

template <typename T>
struct flatten<mp_list<T>> {
    using res = mp_list<T>;
};

template <typename First, typename... Rest>
struct flatten<mp_list<First, Rest...>> {
    using first = mp_list<First>;
    using rest = typename flatten<Rest...>::res;
    static_assert(is_mp_list<rest>::value);
    using res = mp_append<first, rest>;
};

template <typename T, size_t N, bool cond>
struct prefix;

template <typename... Ts>
struct prefix<mp_list<Ts...>, 0, false> {
    using ans = mp_list<Ts...>;
};

template <typename... Ts>
struct prefix<mp_list<Ts...>, 0, true> {
    using ans = mp_list<>;
};

// recursive case: N > 0
template <typename T, typename... Ts, size_t N>
struct prefix<mp_list<T, Ts...>, N, false> {
    using ans = mp_flatten<mp_list<T, typename prefix<mp_list<Ts...>, N - 1, ((N - 1) == 0)>::ans>>;
};


template <typename T> 
struct removeZeros {};

template <typename T, typename... Ts>
struct removeZeros<mp_list<T, Ts...>> {
    using ans = std::conditional_t<std::is_same_v<T, Zero>, 
        typename removeZeros<mp_list<Ts...>>::ans, mp_list<T, Ts...>>;
};
template <>
struct removeZeros<mp_list<>> {
    using ans = mp_list<>;
};

template <typename T, size_t N, bool cond>
struct appendZeros {};

template <typename... Ts, size_t N>
struct appendZeros<mp_list<Ts...>, N, false> {
    using ans = typename prefix<mp_list<Ts...>, N, N == 0>::ans;
};

template <typename... Ts, size_t N>
struct appendZeros<mp_list<Ts...>, N, true> {
    using ans = mp_append<mp_list<Ts...>, mp_repeat_c<mp_list<Zero>, N>>;
};

#endif