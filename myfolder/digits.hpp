#ifndef DIGITS_HPP
#define DIGITS_HPP
#include <queue>

struct Digit {

};
struct Zero : public Digit  {
    using num = int;
};
struct One : public Digit  {
    using num = int;
};

struct Two : public Digit {
    using num = std::queue<int>;
};

struct Three : public Digit {
    using num = int;
};

struct Four : public Digit {
    using num = int;
};

struct Five : public Digit {
    using num = int;
};

struct Six : public Digit {
    using num = int;
};
struct Seven : public Digit {
    using num = int;
};
struct Eight : public Digit {
    using num = int;
};
struct Nine : public Digit{
    using num = int;
};

#endif