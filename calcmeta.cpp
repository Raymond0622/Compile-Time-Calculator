#include <iostream>
#include <type_traits>
#include <queue>

//digits
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
template <typename T, typename = void>
struct is_digit : std::false_type {};

template <typename T>
struct is_digit<T, std::void_t<typename T::num>> : std::true_type {};

struct Operation {
    using ops = std::vector<int>;
};
struct Add : public Operation {
    template <typename T, typename U>
    static auto add(T one, U two) {
        if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Zero>) {
            return new One();
        }
        if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), One>) {
            return new Two();
        }
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Two>) {
            return new Three();
        }
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Three>) {
            return new Four();
        }
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Nine>) {
            return new Zero();
        }
    }  
};

template <typename Ops, typename... Args>
auto arthimetic(Args&&... args) {
    if constexpr (std::is_same_v<Ops, Add>) {
        return Add::add(std::forward<Args>(args)...);
    }
}
int main() {
    One one;
    One one1;
    Two two;
    auto d = arthimetic<Add>(one, two);
    static_assert(std::is_same_v<Three*, decltype(d)>);
    static_assert(is_digit<One>::value);
}