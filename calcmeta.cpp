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

int main() {

    using number1 = mp_reverse<typename CharToDigit<NUMBER1>::digits>;
    using number2 = mp_reverse<typename CharToDigit<NUMBER2>::digits>;
    
    //std::cout << RecursiveSubtract<number1, number2>::value << std::endl;

    #if defined(MULTIPLY)
        using ans = typename Multiply<number1, number2, 0>::ans;
        using final = typename RecursiveAdd<ans>::ans;
        printAll<number1, number2, final, true, true, true, -1>(ops);
   
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
        
        printAll<number1, number2, final, true, true, val::value && (n1 >= n2), -1>(ops);
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