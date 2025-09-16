#include <iostream>
#include <type_traits>
#include "boost/mp11.hpp"
#include "myfolder/digits.hpp"
#include <queue>

template <typename T, typename U>
struct Sum {
    using Carry = std::conditional_t<std::is_same_v<U, One>, One, Zero>;
    using Remainder = T;
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
            return new Sum<One, Zero>;
        }
        if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), One>) {
            return new Sum<Two, Zero>;
        }
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Two>) {
            return new Sum<Three, Zero>;
        }
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Three>) {
            return new Sum<Four, Zero>;
        }
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Nine>) {
            return new Sum<One, Zero>;
        }
    }  
};

template <typename Ops, typename... Args>
auto arthimetic(Args&&... args) {
    if constexpr (std::is_same_v<Ops, Add>) {
        return Add::add(std::forward<Args>(args)...);
    }
}
using namespace boost::mp11;

int main() {
    One one;
    One one1;
    Two two;
    Three three;
    // Use Boost to store the digits of two numbers
    using number1 = mp_list<One, Two, Three>;
    

    // 3. Access by index
    static_assert(std::is_same_v<mp_at_c<number1, 0>, One>);
    static_assert(std::is_same_v<mp_at_c<my_types, 1>, Two>);
    auto d = arthimetic<Add>(one, two);
    static_assert(is_digit<One>::value);
}