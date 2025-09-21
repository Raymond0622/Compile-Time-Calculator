#include <iostream>
#include <type_traits>
#include <typeinfo>
#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"
#include "Digits.hpp"
#include "Sum.hpp"
#include "Carry.hpp"
#include "Operations.hpp"

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

template <typename D1, typename D2>
struct multiply_digit {
    static_assert(std::is_base_of_v<Digit, D1> && 
        std::is_base_of_v<Digit, D2>, "not a digit");
    using curr = MultiplyDigits<D1, D2>;
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
    using first = mp_list<First>;
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
    using c = std::conditional_t<is_carry<D1_carry>::value, typename D1_carry::carry, Zero>;
    
    using rest = Add<Number1<D1s...>, Number2<D2s...>, 
        std::conditional_t<std::is_same_v<c, One> || 
        std::is_same_v<typename curr::Carry, One>, One, Zero>>;
    using ans = mp_list<typename curr::Remainder, typename rest::ans>;
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

template <typename T>
struct RecursiveAdd {};

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
    using a = mp_append<Number1<D1s...>, mp_repeat_c<mp_list<Zero>, mp_size<Number2<D2s...>>::value - mp_size<Number1<D1s...>>::value>>;
    using res = typename flatten<typename Add<a, Number2<D2s...>, Zero>::ans>::res; 
    using ans = typename RecursiveAdd<mp_list<res, Ts...>>::ans;
};

int main() {
    // Use Boost to store the digits of two numbers, number1 >= number2 requirement (for addition only)
    using number1 = mp_list<One, Two, Three, Four>;
    using number2 = mp_list<Four, Three, Five, Six, Seven, Four>;
    constexpr size_t n1 = mp_size<number1>::value;
    constexpr size_t n2 = mp_size<number2>::value;

    //using num2 = mp_append<number2, mp_repeat_c<mp_list<Zero>, n1 - n2>>;
    //using num1 = number1;
    //constexpr size_t n3 = mp_size<num2>::value;
    using ans = typename Multiply<mp_reverse<number1>, mp_reverse<number2>, 0>::ans;
    //std::cout << mp_size<ans>::value;
    
    // mp_for_each<mp_reverse<number1>>([] (auto d) {
    //     print(d);
    // });
    // std::cout << " * ";
    // mp_for_each<mp_reverse<number2>>([] (auto d) {
    //     print(d);
    // });
    // std::cout << " = ";
    // // Print out the results
    using final = typename RecursiveAdd<ans>::ans;

    mp_for_each<ans>([](auto I) {
        mp_for_each<mp_reverse<decltype(I)>>([](auto d) {
           print(d);
           
     });
     std::cout << ' ' << std::endl;
    });
    // // using t = typename RecursiveAdd<ans>::a;
    // // mp_for_each<t>([] (auto I) {
    // //     print(I);
    // // });

    mp_for_each<mp_reverse<final>>([](auto I) {   
        print(I);        
     });
}