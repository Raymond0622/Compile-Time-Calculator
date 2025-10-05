#ifndef PRINT_HPP
#define PRINT_HPP

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/type_index.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/mp11.hpp>

using namespace::boost::mp11;
struct Neg {};

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
    else if constexpr (std::is_same_v<Zero, T>){
        std::cout << "0";
    }
    else {
        std::cout << "-";
    }
}

template <typename Number1, typename Number2, typename Final, 
    bool isPositive1, bool isPositive2, bool isPositive, int decimal>
void printAll(const char& ops) {
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
    int count = 0;
    mp_for_each<mp_reverse<f>>([&](auto I) {   
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
};
template <typename T>
void printSingle() {
    mp_for_each<T>([] (auto d) {
        print(d);
    });
}

#endif
