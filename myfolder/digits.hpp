#ifndef DIGITS_HPP
#define DIGITS_HPP
struct Digit {};
struct Zero : public Digit  {
    using D = Zero;
};
struct One : public Digit  {
    using D = One;
};

struct Two : public Digit {
    using D = Two;
};

struct Three : public Digit {
    using D = Three;
};

struct Four : public Digit {
    using D = Four;
};
struct Five : public Digit {
    using D = Five;
};

struct Six : public Digit {
    using D = Six;
};
struct Seven : public Digit {
    using D = Seven;
};
struct Eight : public Digit {
    using D = Eight;
};
struct Nine : public Digit{
    using D = Nine;
};

#endif