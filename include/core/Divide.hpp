#ifndef DIVIDE_HPP
#define DIVIDE_HPP

#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"

template <typename T, typename U, size_t N>
struct Divide {};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Number2, size_t N>
struct Divide<Number1<D1s...>, Number2<D2s...>, N> {

    constexpr static int n1 = mp_size<Number1<D1s...>>::value;
    constexpr static int n2 = mp_size<Number2<D2s...>>::value;
    constexpr static int diff = (n1 < n2) ? n2 - n1 : n2;
    using dividend = typename appendZeros<Number1<D1s...>, diff, n1 < n2>::ans;

    // annoying dealing with std::conditional_t
    using interm = std::conditional_t<(n1 <= n2), dividend, Number1<D1s...>>;
    constexpr static int interm_val = (n1 <= n2) ? 1 : (n2 + 1);
    using c = Compare<dividend, Number2<D2s...>>;
    using dividend_f = std::conditional_t<c::ans::value,
         dividend, typename appendZeros<interm, interm_val, (n1 <= n2)>::ans>;
        
    constexpr static int df_val = mp_size<dividend_f>::value;
    constexpr static int diff2 = (df_val - n2 == 0) ? 0 : df_val - n2;
    using num2 = mp_reverse<mp_append<mp_reverse<Number2<D2s...>>, mp_repeat_c<mp_list<Zero>, diff2>>>;
    
    using curr = RecursiveSubtract<mp_reverse<dividend_f>, mp_reverse<num2>, true>;
    using next_d = typename removeZeros<mp_reverse<typename curr::ans>>::ans;
    using next = Divide<next_d, Number2<D2s...>, N - 1>;
    constexpr static int pad = (n1 < n2) ? n2 - n1 - 1 + (1 - c::ans::value) : 0;
    using ans = mp_flatten<mp_append<mp_repeat_c<mp_list<Zero>, pad>, mp_list<typename convertIntDigit<curr::value>::D>,
    typename next::ans>>;
    constexpr static int decimal = ((n1 < n2) || (!c::ans::value && (n1 <= n2))) ? PRECISION - N : next::decimal;
    // constexpr static int decimal = -1;
    
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Number2>
struct Divide<Number1<D1s...>, Number2<D2s...>, 0> {
    constexpr static int decimal = -1;
    using ans = mp_list<>;
    
};

template <template <typename...> class Number1,
typename... D2s, template <typename...> class Number2, size_t N>
struct Divide<Number1<>, Number2<D2s...>, N> {
    constexpr static int decimal = PRECISION - N;
    using ans = mp_list<>;
    
};

#endif