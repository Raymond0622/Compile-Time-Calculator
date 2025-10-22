#ifndef ADD_HPP
#define ADD_HPP

#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"

using namespace boost::mp11;

template <typename D1, typename D2>
struct add_digit {
    static_assert(std::is_base_of_v<Digit, D1> && 
        std::is_base_of_v<Digit, D2>, "not a digit");
    using curr = AddDigits<D1, D2>;
};

template <typename T, typename U, typename = void>
struct Add {};

template <typename T, typename U, typename Carry>
struct Add<mp_list<T>, mp_list<U>, Carry> {
    //static_assert(std::is_same_v<Carry, One>);
    using D1_carry = std::conditional_t<std::is_same_v<Carry, One>, Carried<T>, typename T::D>;
    using curr = typename add_digit<typename D1_carry::D, U>::curr::type;
    using c = std::conditional_t<is_carry<D1_carry>::value, typename D1_carry::carry, Zero>;
    using rest = std::conditional_t<std::is_same_v<c, One> || 
        std::is_same_v<typename curr::Carry, One>, One, Zero>;
    using ans = std::conditional_t<std::is_same_v<rest, One>, 
        mp_list<typename curr::Remainder, mp_list<rest>>, mp_list<typename curr::Remainder>> ;
};

template <typename D1, typename... D1s, template <typename...> class Number1, 
          typename D2, typename... D2s, template <typename...> class Number2, typename Carry>
struct Add<Number1<D1, D1s...>, Number2<D2, D2s...>, Carry> {
    using D1_carry = std::conditional_t<std::is_same_v<Carry, One>, Carried<D1>, typename D1::D>;
    using curr = typename add_digit<typename D1_carry::D, D2>::curr::type;
    using c = std::conditional_t<is_carry<D1_carry>::value, typename D1_carry::carry, Zero>;
    
    using rest = Add<Number1<D1s...>, Number2<D2s...>, 
        std::conditional_t<std::is_same_v<c, One> || 
        std::is_same_v<typename curr::Carry, One>, One, Zero>>;
    using ans = mp_list<typename curr::Remainder, typename rest::ans>;
};

template <typename T>
struct RecursiveAdd {};

template <typename... D1s, template <typename...> class Number1>
struct RecursiveAdd<mp_list<Number1<D1s...>>> {
    using ans = mp_flatten<Number1<D1s...>>;
};

template <typename... D1s, template <typename...> class Number1, 
        typename... D2s, template <typename...> class Number2>
struct RecursiveAdd<mp_list<Number1<D1s...>, Number2<D2s...>>>{
    // utilize the fact that size of Number1 <= Number 2
    using a = mp_append<Number1<D1s...>, mp_repeat_c<mp_list<Zero>, mp_size<Number2<D2s...>>::value - mp_size<Number1<D1s...>>::value>>;
    using ans = typename flatten<typename Add<a, Number2<D2s...>, Zero>::ans>::res;
};

template <typename... D1s, template <typename...> class Number1, 
        typename... D2s, template <typename...> class Number2, typename... Ts> 
struct RecursiveAdd<mp_list<Number1<D1s...>, Number2<D2s...>, Ts...>>{
    // utilize the fact that size of Number1 <= Number 2
    static constexpr int decimal = -1;
    using a = mp_append<Number1<D1s...>, mp_repeat_c<mp_list<Zero>, mp_size<Number2<D2s...>>::value - mp_size<Number1<D1s...>>::value>>;
    using res = typename flatten<typename Add<a, Number2<D2s...>, Zero>::ans>::res; 
    using ans = typename RecursiveAdd<mp_list<res, Ts...>>::ans;
};


#endif 