#include <iostream>
#include <type_traits>
#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"
#include "myfolder/digits.hpp"
#include "myfolder/SumResult.hpp"
#include "myfolder/Carry.hpp"
#include <typeinfo>
#include <queue>

using namespace boost::mp11;

template <typename T, typename = void>
struct is_digit : std::false_type {};

template <typename T>
struct is_digit<T, std::void_t<typename T::c>> : std::true_type {};

template <typename D1, typename D2>
struct add_digit {
    static_assert(std::is_base_of_v<Digit, D1> && 
        std::is_base_of_v<Digit, D2>, "not a digit");
    using curr = AddDigits<D1, D2>;
};

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
    //using first = std::conditional_t<is_mp_list<First>::value, typename flatten<First>::res, mp_list<First>>;
    using first = mp_list<First>;
    //static_assert(is_mp_list<first>::value);
    using rest = typename flatten<Rest...>::res;
    static_assert(is_mp_list<rest>::value);
    using res = mp_append<first, rest>;
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
    //static_assert(std::is_same_v<curr, Sum<Zero, One>>);
    using c = std::conditional_t<is_carry<D1_carry>::value, typename D1_carry::carry, Zero>;
    
    using rest = Add<Number1<D1s...>, Number2<D2s...>, 
        std::conditional_t<std::is_same_v<c, One> || 
        std::is_same_v<typename curr::Carry, One>, One, Zero>>;
    using ans = mp_list<typename curr::Remainder, typename rest::ans>;
};

template <typename T>
void print(T sum) {
    if constexpr (std::is_same_v<One, T>) {
        std::cout << "1";
    }
    else if constexpr (std::is_same_v<Two, T>) {
        std::cout << "2";
    }
    else if constexpr (std::is_same_v<Three, T>) {
        std::cout << "3";
    }
    else if constexpr (std::is_same_v<Four, T>) {
        std::cout << "4";
    }
    else if constexpr (std::is_same_v<Five, T>) {
        std::cout << "5";
    }
    else if constexpr (std::is_same_v<Six, T>) {
        std::cout << "6";
    }
    else if constexpr (std::is_same_v<Seven, T>) {
        std::cout << "7";
    }
    else if constexpr (std::is_same_v<Eight, T>) {
        std::cout << "8";
    }
    else if constexpr (std::is_same_v<Nine, T>) {
        std::cout << "9";
    }
    else {
        std::cout << "0";
    }
}

int main() {
    // Use Boost to store the digits of two numbers
    using number1 = mp_list<One, Nine, Eight, Nine>;
    using number2 = mp_list<One, Nine, Nine, Four>;
    using result = typename Add<number1, number2, Zero>::ans;
    using ans = flatten<result>::res;
    //constexpr size_t i = mp_size<ans>::value;
    //std::cout << i << std::endl;

    mp_for_each<mp_reverse<number1>>([] (auto d) {
        print(d);
    });
    std::cout << " + ";
    mp_for_each<mp_reverse<number2>>([] (auto d) {
        print(d);
    });
    std::cout << " = ";
    // Print out the results
    mp_for_each<mp_reverse<ans>>([](auto d) {
        print(d);
    });
}