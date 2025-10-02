#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <cstring>
#include <utility> 
#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"
#include "Digits.hpp"
#include "Sum.hpp"
#include "Carry.hpp"
#include "Operations.hpp"

constexpr char ops = OPERATION[0];

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

template <typename D1, typename D2>
struct subtract_digit {
    // always subtract D1 - D2
    static_assert(std::is_base_of_v<Digit, D1> && 
        std::is_base_of_v<Digit, D2>, "not a digit");
    using curr = SubtractDigits<D1, D2>;
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

template <typename T, typename U, bool cond>
struct RecursiveSubtract {
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Number2>
struct RecursiveSubtract<Number1<D1s...>, Number2<D2s...>, true> {
        //10, 123
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
    using dividend_f = std::conditional_t<Compare<dividend, Number2<D2s...>>::ans::value,
         dividend, typename appendZeros<interm, interm_val, (n1 <= n2)>::ans>;
        
    
    constexpr static int df_val = mp_size<dividend_f>::value;
    constexpr static int diff2 = (df_val - n2 == 0) ? 0 : df_val - n2;
    using num2 = mp_reverse<mp_append<mp_reverse<Number2<D2s...>>, mp_repeat_c<mp_list<Zero>, diff2>>>;
    
    using curr = RecursiveSubtract<mp_reverse<dividend_f>, mp_reverse<num2>, true>;
    using next_d = typename removeZeros<mp_reverse<typename curr::ans>>::ans;
    using next = Divide<next_d, Number2<D2s...>, N - 1>;
    using ans = mp_flatten<mp_list<typename convertIntDigit<curr::value>::D, typename next::ans>>;
    constexpr static int decimal = ((n1 < n2) || !Compare<dividend, Number2<D2s...>>::ans::value) ? 
         : next::decimal;
    
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Number2>
struct Divide<Number1<D1s...>, Number2<D2s...>, 0> {
    constexpr static int decimal = -1;
    using ans = mp_list<>;
    
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
    using a = mp_append<Number1<D1s...>, mp_repeat_c<mp_list<Zero>, mp_size<Number2<D2s...>>::value - mp_size<Number1<D1s...>>::value>>;
    using res = typename flatten<typename Add<a, Number2<D2s...>, Zero>::ans>::res; 
    using ans = typename RecursiveAdd<mp_list<res, Ts...>>::ans;
};

template <long long N>
struct CreateDigits {
    using ans = mp_flatten<mp_list<typename convertIntDigit<N % 10>::D, typename CreateDigits<N / 10>::ans>>;
};
template <>
struct CreateDigits<0> {
    using ans = mp_list<>;
};

template <typename T, typename U, bool useT = false>
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

struct Neg {};

template <typename Number1, typename Number2, typename Final, 
    bool isPositive1, bool isPositive2, bool isPositive>
void print() {
    using n1 = mp_append<Number1, std::conditional_t<isPositive1, mp_list<>, mp_list<Neg>>>;
    using n2 = mp_append<Number2, std::conditional_t<isPositive2, mp_list<>, mp_list<Neg>>>;
    using f = mp_append<Final, std::conditional_t<isPositive, mp_list<>, mp_list<Neg>>>;
    
    mp_for_each<mp_reverse<n1>>([] (auto d) {
        print(d);
    });

    std::cout << " " << OPERATION << " ";
    mp_for_each<mp_reverse<n2>>([] (auto d) {
        print(d);
    });
    
    std::cout << " = ";
    bool found = false;
    mp_for_each<mp_reverse<f>>([&](auto I) {   
        if constexpr (!std::is_same_v<decltype(I), Zero>) {
            found = true;
        }
        if (found)
            print(I);        
    });

};
template <typename T>
void printSingle() {
    mp_for_each<T>([] (auto d) {
        print(d);
    });
}

template <size_t N>
struct fixed_string {
    char value[N];

    constexpr fixed_string(char const (&str)[N]) {
        for (std::size_t i = 0; i < N; ++i)
            value[i] = str[i];
    }
};

template <fixed_string S, typename T>
struct DigitToMPList {};

template <fixed_string S, size_t... I>
struct DigitToMPList<S, std::index_sequence<I...>> {
    using mplist = mp_list<typename convertIntDigit<S.value[I] - '0'>::D...>;
};

template <fixed_string S>
struct CharToDigit {
    using digits = typename 
        DigitToMPList<S, std::make_index_sequence<sizeof(S.value) - 1>>::mplist;
};

int main() {

    using number1 = mp_reverse<typename CharToDigit<NUMBER1>::digits>;
    using number2 = mp_reverse<typename CharToDigit<NUMBER2>::digits>;
    
    using C = Divide<mp_reverse<number1>, mp_reverse<number2>, 10>;
    // printSingle<typename C::dividend>();
    // std::cout <<std::endl;
    // printSingle<typename C::dividend_f>();
    // std::cout << std::endl;
    // printSingle<typename C::num2>();
    // std::cout << std::endl;
    // using L = typename C::curr;
    using G = typename C::dividend;
    printSingle<G>();
    std::cout << std::endl;
    printSingle<typename C::interm>();
    std::cout << std::endl;
    printSingle<typename C::dividend_f>();
    std::cout << std::endl;
    std::cout << C::df_val << std::endl;
    std::cout << C::diff2 << std::endl;
    //using L = typename C::curr;
    printSingle<typename C::ans>();
    std::cout << std::endl;
    std::cout << C::decimal << std::endl;
    
    //std::cout << RecursiveSubtract<number1, number2>::value << std::endl;

    #if defined(MULTIPLY)
        using ans = typename Multiply<number1, number2, 0>::ans;
        using final = typename RecursiveAdd<ans>::ans;
        //print<number1, number2, final, true, true, true>();
   
    #elif defined(ADD)

        constexpr size_t n1 = mp_size<number1>::value;
        constexpr size_t n2 = mp_size<number2>::value;
        // Need to swap since the implementation requires the same number of digits
        using num1 = typename Swap<number1, number2, (n1 > n2)>::num1;
        using num2 = typename Swap<number1, number2, (n1 > n2)>::num2;
        using zeros = mp_repeat_c<mp_list<Zero>, (mp_size<num1>::value - mp_size<num2>::value)>;
        using alter = mp_append<num2, zeros>;
        using final = typename flatten<typename Add<num1, alter, Zero>::ans>::res;
        print<number1, number2, final, true, true, true>();

    #elif defined(SUBTRACT)
        // This requires two steps:
        // 1. Perform padding to fill the two numbers with equal number of digits
        // 2. Compare the two digits to see which one is bigger
        
        // This require two steps because to compare which digit is bigger,
        // Need to swap based on # of digits, and padd the smaller lesser digits with 0's
        // Need to do this to compare the numbers digit by digit.
        // Sure, we can compare just which number has more digits, and it's the greater one,
        // But, what happens if the # of digits are the same. We still have to compare digit.
        // So to consider both cases (same digits, different digits), need both step
        constexpr size_t n1 = mp_size<number1>::value;
        constexpr size_t n2 = mp_size<number2>::value;
        using num1 = typename Swap<number1, number2, (n1 >= n2)>::num1;
        using num2 = typename Swap<number1, number2, (n1 >= n2)>::num2;
        using zeros = mp_repeat_c<mp_list<Zero>, (mp_size<num1>::value - mp_size<num2>::value)>;
        using alter = mp_append<num2, zeros>;
        
        // Next, we compare the digits which one is bigger, obviously starting from 
        // the most significant digit
        using val = typename Compare<mp_reverse<num1>, mp_reverse<alter>>::ans;
        using sub1 = typename Swap<num1, alter, val::value>::num1;
        using sub2 = typename Swap<num1, alter, val::value>::num2;
        using final = mp_flatten<typename Subtract<sub1, sub2, Zero>::ans>;
        
        print<number1, number2, final, true, true, val::value && (n1 >= n2)>();
   
    #endif

}