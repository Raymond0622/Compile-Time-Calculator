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
#include "Print.hpp"
#include "Miscellaenous.hpp"
#include "TypeList.hpp"
#include "core/Add.hpp"
#include "core/Subtract.hpp"
#include "core/Divide.hpp"
#include "core/Multiply.hpp"

constexpr char ops = OPERATION[0];

using namespace boost::mp11;

template <typename T, typename U>
struct BinarySingle {};

template <typename... D1s, template <typename...> class Number1, typename... D2s,template <typename...> class Number2>
struct BinarySingle<Number1<D1s...>, Number2<D2s...>> {
    using c = typename Compare<Number1<D1s...>, Number2<D2s...>>::ans;
    using digit = std::conditional_t<c::value, One, Zero>;
    using next = std::conditional_t<c::value, Subtract<Number1<D1s...>, 
        Number2<D2s...>, Zero>, Number1<D1s...>>;
    using ans = typename removeZeros<next>::ans;
};

template <typename T, typename U, size_t N, bool isZero, bool cond>
struct MaxBinary {};

// find MSB recursively (as always), use prior calculated (Previous struct)
// for next iteration
template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Previous, size_t N>
struct MaxBinary<Number1<D1s...>, Previous<D2s...>, N, false, true> {
    using d = typename Multiply<mp_reverse<Previous<D2s...>>, mp_list<Two>, 0>::ans;
    using next = mp_reverse<typename RecursiveAdd<d>::ans>;
    constexpr static int diff = mp_size<Number1<D1s...>>::value - mp_size<next>::value;

    constexpr static int n_diff = mp_size<next>::value - mp_size<Number1<D1s...>>::value;
    using bin = typename appendZerosBinary<next, diff, diff >= 0>::ans;
    using num1 = typename appendZerosBinary<Number1<D1s...>, n_diff, n_diff >= 0>::ans;
    using cond = typename Compare<num1, bin>::ans;
    //static_assert(cond::value);
    using curr = MaxBinary<Number1<D1s...>, next, N + 1, false, cond::value>;
    using ans = typename curr::ans;
    constexpr static size_t val = curr::val;
         
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Previous, int N>
struct MaxBinary<Number1<D1s...>, Previous<D2s...>, N, false, false> {

    constexpr static size_t n11 = mp_size<Previous<D2s...>>::value;
    constexpr static size_t n22 = mp_size<mp_list<Two>>::value;
    constexpr static size_t offset = (n11 > n22) ? n11 - n22 : 0;
    constexpr static size_t val = N - 1;
    using ans = typename Divide<Previous<D2s...>, mp_append<mp_list<Two>,
        mp_repeat_c<mp_list<Zero>, offset>>, PRECISION>::ans;
         
};


template <typename... D1s, template <typename...> class Number1, typename U, size_t N>
struct MaxBinary<Number1<D1s...>, U, N, true, false> {
    using ans = Zero;
};

// cond checks if Number1 is a One
template <typename T, bool cond>
struct Binary {};

template <typename... D1s, template <typename...> class Number1>
struct Binary<Number1<D1s...>, false> {
    using curr = MaxBinary<Number1<D1s...>, mp_list<One>, 0, std::is_same_v<mp_list<Zero>, mp_list<D1s...>>,
        !std::is_same_v<mp_list<Zero>, mp_list<D1s...>>>;
    using bin = typename curr::ans;
    constexpr static size_t diff = mp_size<Number1<D1s...>>::value - mp_size<bin>::value;
    
    using n_bin = mp_append<mp_repeat_c<mp_list<Zero>, diff>, bin>;
    using next = mp_reverse<mp_flatten<typename Subtract<mp_reverse<Number1<D1s...>>, mp_reverse<n_bin>, Zero>::ans>>;
    using next_bin = Binary<next, std::is_same_v<mp_list<One>, next> ||         std::is_same_v<mp_list<Zero>, next>>;
    using ans = mp_flatten<mp_list<mp_list_c<int, curr::val + 1>, typename next_bin::ans>>;
    
};

template <typename... D1s, template <typename...> class Number1>
struct Binary<Number1<D1s...>, true> {
    using ans = std::conditional_t<std::is_same_v<Number1<D1s...>, mp_list<Zero>>, mp_list_c<int, 0>, mp_list_c<int, 1>>;
    
};

template <typename T>
struct ReconstructBinary {};

template <typename A, typename B, typename... Cs, template <typename...> class Binaries>
struct ReconstructBinary<Binaries<A, B, Cs...>> {
    //using ans = mp_list<One, mp_repeat_c<Zero, A::value - B::value - 1>>;
    using gap = mp_repeat_c<mp_list<Zero>, A::value - B::value - 1>;
    using head = mp_list<One>;
    using combined = mp_append<head, gap>;
    using ans = mp_append<combined, typename ReconstructBinary<Binaries<B, Cs...>>::ans>;
};

template <typename A, template <typename...> class Binaries>
struct ReconstructBinary<Binaries<A>> {
    using ans = std::conditional_t<A::value == 1, mp_list<One>, mp_list<>>;
    //using ans = mp_append<mp_list<One>, mp_repeat_c<mp_list<Zero>, A::value - 0>>;
};



int main() {

    using number1 = mp_reverse<typename CharToDigit<NUMBER1>::digits>;
    using number2 = mp_reverse<typename CharToDigit<NUMBER2>::digits>;

    using p = typename Binary<mp_reverse<number1>, false>::ans;
    //std::cout << mp_size<p>::value << std::endl;
    mp_for_each<p>([] (auto I) {
        std::cout << decltype(I)::value << std::endl;
    });
    using ans = ReconstructBinary<p>::ans;
    printSingle<ans>();
    
    //std::cout << RecursiveSubtract<number1, number2>::value << std::endl;

    #if defined(MULTIPLY)
        using ans = typename Multiply<number1, number2, 0>::ans;
        using final = typename RecursiveAdd<ans>::ans;
        //printAll<number1, number2, final, true, true, true, -1>(ops);
   
    #elif defined(ADD)

        constexpr size_t n1 = mp_size<number1>::value;
        constexpr size_t n2 = mp_size<number2>::value;
        // Need to swap since the implementation requires the same number of digits
        using num1 = typename Swap<number1, number2, (n1 > n2)>::num1;
        using num2 = typename Swap<number1, number2, (n1 > n2)>::num2;
        using zeros = mp_repeat_c<mp_list<Zero>, (mp_size<num1>::value - mp_size<num2>::value)>;
        using alter = mp_append<num2, zeros>;
        using final = typename flatten<typename Add<num1, alter, Zero>::ans>::res;
        printAll<number1, number2, final, true, true, true, -1>(ops);

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
        
        //printAll<number1, number2, final, true, true, val::value && (n1 >= n2), -1>(ops);
    #elif defined(DIVIDE)
        constexpr size_t n11 = mp_size<number1>::value;
        constexpr size_t n22 = mp_size<number2>::value;
        constexpr size_t offset = (n11 > n22) ? n11 - n22 : 0;
        
        using C = Divide<mp_reverse<number1>, mp_append<mp_reverse<number2>,
            mp_repeat_c<mp_list<Zero>, offset>>, PRECISION>;
        using final = C::ans;

        printAll<number1, number2, mp_reverse<final>, true, true, true, offset + C::decimal>(ops);
   
    #endif

}