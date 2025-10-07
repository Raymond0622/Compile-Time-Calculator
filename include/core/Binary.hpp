#ifndef BINARY_HPP
#define BINARY_HPP

template <typename T, typename U, size_t N, bool isZero, bool cond>
struct MaxBinary {};

// find MSB recursively (as always), use prior calculated (Previous struct)
// for next iteration
template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Previous, size_t N>
struct MaxBinary<Number1<D1s...>, Previous<D2s...>, N, false, true> {
    using d = typename Multiply<mp_reverse<Previous<D2s...>>, mp_list<Two>, 0>::ans;
    using next = mp_reverse<typename RecursiveAdd<d>::ans>;
    constexpr static int diff = mp_size<Number1<D1s...>>::value - mp_size<next>::value;

    constexpr static int n_diff = mp_size<next>::value - mp_size<Number1<D1s...>>::value;
    using bin = typename appendZerosBinary<next, diff, diff >= 0>::ans;
    using num1 = typename appendZerosBinary<Number1<D1s...>, n_diff, n_diff >= 0>::ans;
    using cond = typename Compare<num1, bin>::ans;
    //static_assert(cond::value);
    using curr = MaxBinary<Number1<D1s...>, next, N + 1, false, cond::value>;
    using ans = typename curr::ans;
    constexpr static size_t val = curr::val;
         
};

template <typename... D1s, template <typename...> class Number1,
typename... D2s, template <typename...> class Previous, int N>
struct MaxBinary<Number1<D1s...>, Previous<D2s...>, N, false, false> {

    constexpr static size_t n11 = mp_size<Previous<D2s...>>::value;
    constexpr static size_t n22 = mp_size<mp_list<Two>>::value;
    constexpr static size_t offset = (n11 > n22) ? n11 - n22 : 0;
    constexpr static size_t val = N - 1;
    using ans = typename Divide<Previous<D2s...>, mp_append<mp_list<Two>,
        mp_repeat_c<mp_list<Zero>, offset>>, PRECISION + 10>::ans;
         
};


template <typename... D1s, template <typename...> class Number1, typename U, size_t N>
struct MaxBinary<Number1<D1s...>, U, N, true, false> {
    using ans = Zero;
};

// cond checks if Number1 is a One
template <typename T, bool cond>
struct Binary {};

template <typename... D1s, template <typename...> class Number1>
struct Binary<Number1<D1s...>, false> {
    using curr = MaxBinary<Number1<D1s...>, mp_list<One>, 0, std::is_same_v<mp_list<Zero>, mp_list<D1s...>>,
        !std::is_same_v<mp_list<Zero>, mp_list<D1s...>>>;
    using bin = typename curr::ans;
    constexpr static size_t diff = mp_size<Number1<D1s...>>::value - mp_size<bin>::value;
    
    using n_bin = mp_append<mp_repeat_c<mp_list<Zero>, diff>, bin>;
    using a = mp_reverse<mp_flatten<typename Subtract<mp_reverse<Number1<D1s...>>, mp_reverse<n_bin>, Zero>::ans>>;
    static_assert(mp_is_list<a>::value);
    //201240112121
    using b = typename removeZeros<a>::ans;
    using next = std::conditional_t<std::is_same_v<mp_list<>, b>, mp_list<Zero>, b>;
    using next_bin = Binary<next, std::is_same_v<mp_list<One>, next> ||         std::is_same_v<mp_list<Zero>, next>>;
    using ans = mp_flatten<mp_list<mp_list_c<int, curr::val + 1>, typename next_bin::ans>>;
    
};

template <typename... D1s, template <typename...> class Number1>
struct Binary<Number1<D1s...>, true> {
    using ans = std::conditional_t<std::is_same_v<Number1<D1s...>, mp_list<Zero>>, mp_list_c<int, 0>, mp_list_c<int, 1>>;
    
};

template <typename T>
struct ReconstructBinary {};

template <typename A, typename B, typename... Cs, template <typename...> class Binaries>
struct ReconstructBinary<Binaries<A, B, Cs...>> {
    //using ans = mp_list<One, mp_repeat_c<Zero, A::value - B::value - 1>>;
    using gap = mp_repeat_c<mp_list<Zero>, A::value - B::value - 1>;
    using head = mp_list<One>;
    using combined = mp_append<head, gap>;
    using ans = mp_append<combined, typename ReconstructBinary<Binaries<B, Cs...>>::ans>;
};

template <typename A, template <typename...> class Binaries>
struct ReconstructBinary<Binaries<A>> {
    using ans = std::conditional_t<A::value == 1, mp_list<One>, mp_list<>>;
};

#endif
