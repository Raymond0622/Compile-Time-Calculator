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
            if (count == Final::decimal) {
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


template <typename... Ts>
struct Polynomial {
    using type = std::tuple<Ts...>;
};

template <typename T, typename U, size_t N> 
struct Monomial {
    constexpr static size_t degree = N;
    using sign = T;
    using value = U;
};

template <typename... Ts>
struct concat_index_sequence {};

template <size_t... Ts>
struct concat_index_sequence<std::index_sequence<Ts...>> {
    using value = std::index_sequence<Ts...>;
};

template <size_t... Ts, size_t... Us, typename... Ps>
struct concat_index_sequence<std::index_sequence<Ts...>, std::index_sequence<Us...>, Ps...> {
    using value = typename concat_index_sequence<std::index_sequence<Ts..., Us...>, Ps...>::value;
};

template <fixed_string S, size_t I>
consteval char char_at() {
    return S.value[I];
}

template <fixed_string S, typename T>
struct CharToMPList {};

template <fixed_string S, size_t... Is>
struct CharToMPList<S, std::index_sequence<Is...>> {
    using polylist = mp_list<typename convertIntDigit<S.value[Is] - '0'>::D...>;
};

template <char ch>
struct IsBinaryOperation : std::bool_constant<
    ch == '+' || ch == '-'> {};

//SFINAE for IsPolyChar does NOT work :(
template <char ch>
struct IsPolyChar : std::bool_constant<
    ch == 'x' || ch == '+' || ch == '-' || (ch - '0' <= 9 && ch - '0' >= 0) || 
        ch == '^'>{};

template <template <char> class Cond, typename Seq, fixed_string S>
struct filter_indices;

template <template <char> class Cond, fixed_string S, size_t... Is>
struct filter_indices<Cond, std::index_sequence<Is...>, S> {
    using type = typename concat_index_sequence<
        std::conditional_t<Cond<S.value[Is]>::value,
            std::index_sequence<Is>, std::index_sequence<>>...
    >::value;
};

template <fixed_string S>
struct CharToPoly {
    using indices = std::make_index_sequence<sizeof(S.value) - 1>;
    using filtered = typename filter_indices<IsPolyChar, indices, S>::type;
    using value = typename CharToMPList<S, filtered>::polylist;
};

template <fixed_string, typename... >
struct getindex {};

template <size_t... Idx, fixed_string S>
struct getindex<S, std::index_sequence<Idx...>> {
    static void printindex() {
        ((std::cout << S.value[Idx]), ...);
    }
};
// takes in mp_list of already filtered char's , filtered chars are
// chars that only have polynomial valid characters
// probably need to filter once more to separate out monomials
template <typename T, typename U>
struct MonomialIndices {};

// this contains list of start of monomials
template <typename T, typename... Ts, size_t... Idx>
struct MonomialIndices<mp_list<T, Ts...>, std::index_sequence<Idx...>> {
    using head = typename concat_index_sequence<std::conditional_t<std::is_same_v<T, Neg> || std::is_same_v<T, Pos>, 
        std::index_sequence<Idx>, std::index_sequence<>>...>::value;
    using value = typename concat_index_sequence<head, std::index_sequence<1 + sizeof...(Ts)>>::value;
};

template <typename T>
struct AdjacentDistance {};

template <size_t Idx1, size_t Idx2, size_t... Idx>
struct AdjacentDistance<std::index_sequence<Idx1, Idx2, Idx...>> {
    using value = typename concat_index_sequence<std::index_sequence<Idx2 - Idx1 - 1>,
        typename AdjacentDistance<std::index_sequence<Idx2, Idx...>>::value>::value;
};

template <size_t Idx>
struct AdjacentDistance<std::index_sequence<Idx>> {
    using value = std::index_sequence<>;
};

template <typename T>
struct CharToMonoPairList {};

template <typename... Ts>
struct CharToMonoPairList<mp_list<Ts...>> {
    using indices = typename MonomialIndices<mp_list<Ts...>, std::make_index_sequence<sizeof...(Ts)>>::value;
    using dist = typename AdjacentDistance<indices>::value;
};

template <typename T, typename U>
struct MonomialIndicesToMonomial {};

template <typename T, typename... Ts, size_t Idx, size_t... Idx>
struct MonomialIndicesToMonomial<mp_list<T, Ts...>, std::index_sequence<Idxs...>> {
    using mono = MonomialConstructor<mp_list<Ts...>, Idx>;
};

struct MonomialList {
    using value = mp_list<PartialMPList<
}

template <typename... Ts, size Idx>
struct PartialMPList<mp_list<Ts...>, Idx> {
   using value = mp_list<std::conditional_t<Idx >= 0, Ts, mp_list<>>...>;
}

// used to construct Monomial struct
// need the sign, coefficient, and power
struct MonomialConstructor {
    using sign = T;
    using coef = MonoCoefficient<mp_list<Ts...>>::value;
    using power = MonoPower<mp_list<Ts...>>::value;
}

template <typename T>
struct MonoPower {};

template <typename... Ts>
struct MonoPower<mp_list<Ts...>> {
    using value = std::conditional_t<std::is_same_v<Variable, T1> && std::is_same_v<Power, T2>, 
        mp_list<Ts...>, typename MonoPowerMPList<T2, Ts...>::value;
};

template <typename D>
struct isDigit : std::bool_constant<D::val >= 0 && D::val <= 9> {};

template <typename T, typename... Ts>
struct MonomialCoefficient<mp_list<T, Ts...> {
    using value = flatten<mp_list<std::conditional_t<isDigit<T>::value, 
        mp_list<T, typename MonomialCoefficient<mp_list<Ts...>>::value>, mp_list<>>>::res;
};

template <fixed_string S>
struct CharToMono {
    using indices = std::make_index_sequence<sizeof(S.value) - 1>;
    using filtered = typename filter_indices<IsPolyChar, indices, S>::type;
    using value = typename CharToMPList<S, filtered>::polylist;
};

int main() {
    fixed_string s(FUNCTION);
    using p = CharToPoly<FUNCTION>;
    getindex<FUNCTION, typename p::filtered>::printindex();

    #if defined(MULTIPLY)
        using Number1 = mp_reverse<typename CharToDigit<NUMBER1>::digits>;
        using Number2 = mp_reverse<typename CharToDigit<NUMBER2>::digits>;
        using ans = typename Multiply<Number1, Number2, 0>::ans;
        using Final = typename RecursiveAdd<ans>::ans;
        Print<PrintBinaryOperation<Number1, Number2>, Scientific<PRECISION, Final>>::result();
        //Print<PrintBinaryOperation<Number1, Number2>, Plain<Final>>::result();
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