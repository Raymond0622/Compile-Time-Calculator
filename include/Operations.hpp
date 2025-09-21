#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

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
    else {
        std::cout << "0";
    }
}

#endif
