#ifndef EXPONENT_HPP
#define EXPONENT_HPP

template <typename T, typename U>
struct Exponent {};

template <typename B, typename Current, typename... Bs>
struct Exponent<mp_list<B, Bs...>, Current> {
    using next = typename Multiply<Current, Current, 0>::ans;

    // reverse ordering since multiply needs reverse ordering
    using final = typename RecursiveAdd<next>::ans; 
    using b = B;
    using ans = mp_append<mp_list<std::conditional_t<std::is_same_v<B, One>, Current, mp_list<>>>,      typename Exponent<mp_list<Bs...>, final>::ans>;
    //using ans = std::conditional_t<std::is_same_v<B, One>, Current, mp_list<>>;
};

template <typename Current>
struct Exponent<mp_list<>, Current> {
    using ans = mp_list<>;
};

// remove any mp_list<> i.e., exponents that do not contribute (bit 0)
template <typename T>
struct CleanBinary {};

template <typename T, typename... Ts>
struct CleanBinary<mp_list<T, Ts...>> {
    using ans = std::conditional_t<std::is_same_v<T, mp_list<>>, 
        typename CleanBinary<mp_list<Ts...>>::ans, mp_append<mp_list<T>, typename CleanBinary<mp_list<Ts...>>::ans>>;
};

template <>
struct CleanBinary<mp_list<>> {
    using ans = mp_list<>;
};

//assume its in reverse order
template <typename T>
struct RecursiveMultiply {};

template <typename T, typename U, typename... Remain>
struct RecursiveMultiply<mp_list<T, U, Remain...>> {
    using m = typename Multiply<T, U, 0>::ans;
    using res = typename RecursiveAdd<m>::ans;
    using ans = typename RecursiveMultiply<mp_list<res, Remain...>>::ans;
};

template <typename T>
struct RecursiveMultiply<mp_list<T>> {
    using ans = T;
};

#endif
