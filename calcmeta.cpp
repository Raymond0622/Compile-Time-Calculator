#include <iostream>
#include <type_traits>
#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"
#include "boost/type_index.hpp"
#include "boost/mp11/utility.hpp"
#include "myfolder/digits.hpp"
#include <typeinfo>
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
        else if constexpr (std::is_same_v<decltype(one), One> && std::is_same_v<decltype(two), Nine>) {
            return new Sum<Zero, One>;
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
template <typename T, typename U, template <typename, typename> class Sum>
void print(Sum<T, U> sum) {
    if constexpr (std::is_same_v<One, T>) {
        std::cout << "1" << std::endl;
    }
    else if constexpr (std::is_same_v<Two, T>) {
        std::cout << "2" << std::endl;
    }
    else if constexpr (std::is_same_v<Three, T>) {
        std::cout << "3" << std::endl;
    }
    else if constexpr (std::is_same_v<Four, T>) {
        std::cout << "4" << std::endl;
    }
    else if constexpr (std::is_same_v<Five, T>) {
        std::cout << "5" << std::endl;
    }
    else if constexpr (std::is_same_v<Six, T>) {
        std::cout << "6" << std::endl;
    }
    else if constexpr (std::is_same_v<Seven, T>) {
        std::cout << "7" << std::endl;
    }
    else if constexpr (std::is_same_v<Eight, T>) {
        std::cout << "8" << std::endl;
    }
    else if constexpr (std::is_same_v<Nine, T>) {
        std::cout << "9" << std::endl;
    }
    else {
        std::cout << "0" << std::endl;
    }
}
template <typename T>
auto carryOne(T digit) {
    if constexpr (std::is_same_v<T, Zero>) {
        return new One();
    }
    else if constexpr (std::is_same_v<T, One>) {
        return new Two();
    } 
}
using namespace boost::mp11;

int main() {
    // One one;
    // One one1;
    // Two two;
    // Three three;
    // Use Boost to store the digits of two numbers
    using number1 = mp_list<One, One>;
    using number2 = mp_list<Nine, Two>;
    using zipped_numbers = mp_transform<mp_list, number1, number2>;

    mp_for_each<zipped_numbers>([](auto t) 
    {
        using digit1 = mp_at_c<decltype(t), 0>;
        using digit2 = mp_at_c<decltype(t), 1>;
        digit1 d1;
        digit2 d2;
        if constexpr (std::is_same_v<typename res::Carry, One>) {
            using n_digit1 = decltype(carryOne(d1));
        }
        using res = std::remove_pointer_t<decltype(arthimetic<Add>(std::move(d1), std::move(d2)))>;
        
        res r;
        print(std::move(r));
    });
    

    // // 3. Access by index
    // auto d = arthimetic<Add>(one, two);
    // static_assert(is_digit<One>::value);
}