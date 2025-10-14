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
#include "core/Binary.hpp"
#include "core/Exponent.hpp"
#include "notation/Scientific.hpp"

constexpr char ops = OPERATION[0];

using namespace boost::mp11;

template <typename PrintOperation, typename Notation>
struct Print : public PrintOperation, public Notation  {
    using Notation::printResult;
    using PrintOperation::print;
    static void result() {
        print();
        printResult();
    }
};


template<typename Number1, typename Number2>
struct PrintBinaryOperation {
    static void print() {
        mp_for_each<mp_reverse<Number1>>([] (auto d) {
            printDigit(d);
        });
    
        std::cout << " " << '*' << " ";
        mp_for_each<mp_reverse<Number2>>([] (auto d) {
            printDigit(d);
        });
        std::cout << " = ";
    }
};

template<typename Number1>
struct PrintSingletonOperation {
    static void print() {
        mp_for_each<mp_reverse<Number1>>([] (auto d) {
            printDigit(d);
        });
        std::cout << " = ";
    }
};

template <typename Final>
struct Plain {
    static bool found;
    static int count;
    static void printResult() {
    mp_for_each<mp_reverse<Final>>([&](auto I) {   
        if (ops == '/') {
            if (count == decimal) {
                std::cout << '.';
            }
            print(I);
        }
        else {
            if constexpr (!std::is_same_v<decltype(I), Zero>) {
                found = true;
            }
            if (found)
                print(I);    
        }
        count++;    
    });
    }
};
template <typename Final>
bool Plain<Final>::found = false;

template <typename Final>
int Plain<Final>::count = 0;

int main() {

    #if defined(MULTIPLY)
        using Number1 = mp_reverse<typename CharToDigit<NUMBER1>::digits>;
        using Number2 = mp_reverse<typename CharToDigit<NUMBER2>::digits>;
        using ans = typename Multiply<Number1, Number2, 0>::ans;
        using Final = typename RecursiveAdd<ans>::ans;
        Print<PrintBinaryOperation<Number1, Number2>, Scientific<PRECISION, Final>>::result();
    
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
    #elif defined(BINARY) 
        using p = typename Binary<mp_reverse<number1>, false>::ans;
        using ans = ReconstructBinary<p>::ans;
        std::cout << "Binary of " << NUMBER1 << ": ";
        printSingle<ans>();

    #elif defined(EXPONENT) 
        using f = typename Binary<mp_reverse<number2>, false>::ans;
        using g = mp_reverse<typename ReconstructBinary<f>::ans>;
        using d = typename Exponent<g, number1>::ans;
        using pl = CleanBinary<d>::ans;
        using final = RecursiveMultiply<pl>::ans;
        std::cout << NUMBER1 << "^" << NUMBER2 << " = ";
        printSingle<mp_reverse<final>>();

    #endif

}