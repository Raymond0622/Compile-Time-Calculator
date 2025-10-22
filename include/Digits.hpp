#ifndef DIGITS_HPP
#define DIGITS_HPP

#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "boost/mp11.hpp"

using namespace boost::mp11;

struct Neg {};
struct Pos {};
struct Variable {};

template <size_t N>
struct fixed_string {
    char value[N];

    constexpr fixed_string(char const (&str)[N]) {
        for (std::size_t i = 0; i < N; ++i)
            value[i] = str[i];
    }
};


template <typename T, typename = void>
struct is_digit : std::false_type {};

template <typename T>
struct is_digit<T, std::void_t<typename T::c>> : std::true_type {};

struct Digit {
    using carry = int;
};

struct Zero : public Digit  {
    using D = Zero;
    static constexpr int val = 0;
};
struct One : public Digit  {
    using D = One;
    static constexpr int val = 1;
};

struct Two : public Digit {
    using D = Two;
    static constexpr int val = 2;
};

struct Three : public Digit {
    using D = Three;
    static constexpr int val = 3;
};

struct Four : public Digit {
    using D = Four;
    static constexpr int val = 4;
};
struct Five : public Digit {
    using D = Five;
    static constexpr int val = 5;
};

struct Six : public Digit {
    using D = Six;
    static constexpr int val = 6;
};
struct Seven : public Digit {
    using D = Seven;
    static constexpr int val = 7;
};
struct Eight : public Digit {
    using D = Eight;
    static constexpr int val = 8;
};
struct Nine : public Digit{
    using D = Nine;
    static constexpr int val = 9;
};
struct Power {};

template <int N>
struct convertIntDigit {};

template <>
struct convertIntDigit<'^' - '0'> {
    using D = Power;
};

template <>
struct convertIntDigit<'+' - '0'> {
    using D = Pos;
};

template <>
struct convertIntDigit<'-' - '0'> {
    using D = Neg;
};

template <>
struct convertIntDigit<'x' - '0'> {
    using D = Variable;
};
template <>
struct convertIntDigit<0> {
    using D = Zero;
};
template <>
struct convertIntDigit<1> {
    using D = One;
};
template <>
struct convertIntDigit<2> {
    using D = Two;
};
template <>
struct convertIntDigit<3> {
    using D = Three;
};
template <>
struct convertIntDigit<4> {
    using D = Four;
};
template <>
struct convertIntDigit<5> {
    using D = Five;
};
template <>
struct convertIntDigit<6> {
    using D = Six;
};
template <>
struct convertIntDigit<7> {
    using D = Seven;
};
template <>
struct convertIntDigit<8> {
    using D = Eight;
};
template <>
struct convertIntDigit<9> {
    using D = Nine;
};

template <long long N>
struct CreateDigits {
    using ans = mp_flatten<mp_list<typename convertIntDigit<N % 10>::D, typename CreateDigits<N / 10>::ans>>;
};
template <>
struct CreateDigits<0> {
    using ans = mp_list<>;
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

#endif