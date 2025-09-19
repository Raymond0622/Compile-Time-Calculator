#ifndef DIGITS_HPP
#define DIGITS_HPP

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

template <size_t N>
struct convertIntDigit {};

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

#endif