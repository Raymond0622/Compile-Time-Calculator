
#ifndef SUBTRACT_HPP
#define SUBTRACT_HPP

#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"

template <typename D1, typename D2>
struct subtract_digit {
    // always subtract D1 - D2
    static_assert(std::is_base_of_v<Digit, D1> && 
        std::is_base_of_v<Digit, D2>, "not a digit");
    using curr = SubtractDigits<D1, D2>;
};

template <typename T, typename U, typename S> 
struct Subtract {};

template <typename D1, template <typename...> class Number1,
    typename D2, template <typename...> class Number2, typename T>
struct Subtract<Number1<D1>, Number2<D2>, T> {
    using sub_before = typename subtract_digit<D1, T>::curr;
    using sub = typename subtract_digit<typename sub_before::type, D2>::curr;
    // using borrow = std::conditional_t<std::is_same_v<typename sub_before::borrow, One> ||
    //         std::is_same_v<typename sub::borrow, One>, One, Zero>;
    using ans = std::conditional_t<std::is_same_v<Zero, typename sub::type>, mp_list<>, 
        mp_list<typename sub::type>>;
};

template <typename D1, typename... D1s, template <typename...> class Number1,
    typename D2, typename... D2s, template <typename...> class Number2, typename T>
struct Subtract<Number1<D1, D1s...>, Number2<D2, D2s...>, T> {
    using sub_before = typename subtract_digit<D1, T>::curr;
    using sub = typename subtract_digit<typename sub_before::type, D2>::curr;
        
    using borrow = std::conditional_t<std::is_same_v<typename sub_before::borrow, One> ||
            std::is_same_v<typename sub::borrow, One>, One, Zero>;
    using ans = mp_list<typename sub::type, 
        mp_flatten<typename Subtract<Number1<D1s...>, Number2<D2s...>, borrow>::ans>>;
};

template <typename T, typename U, bool cond>
struct RecursiveSubtract {
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Number2>
struct RecursiveSubtract<Number1<D1s...>, Number2<D2s...>, true> {
        //10, 123
    using n1 = Number1<D1s...>;
    using n2 = Number2<D2s...>;
    using res = mp_flatten<typename Subtract<Number1<D1s...>, Number2<D2s...>, Zero>::ans>;

    using a = mp_append<res, mp_repeat_c<mp_list<Zero>, 
        mp_size<Number2<D2s...>>::value - mp_size<res>::value>>;
    // // compare if res >= num2 
    using val = typename Compare<mp_reverse<a>, mp_reverse<Number2<D2s...>>>::ans; 
    using next = RecursiveSubtract<a, Number2<D2s...>, val::value>;
    using ans = mp_append<mp_list<>, typename next::ans>;
    static constexpr size_t value = 1 + next::value;
        
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Number2>
struct RecursiveSubtract<Number1<D1s...>, Number2<D2s...>, false> {

    using ans = Number1<D1s...>;
    static constexpr size_t value = 0;
        
};

#endif