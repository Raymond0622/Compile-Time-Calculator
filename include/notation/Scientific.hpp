
#ifndef SCIENTIFIC_HPP
#define SCIENTIFIC_HPP

// Scientific notation with how many significant numbers to use
template <size_t N, typename Final>
struct Scientific {
    static int zeros;
    static bool found;
    static int tot; 
    static void printResult() {
        mp_for_each<mp_reverse<Final>>([&](auto I) {      
            if (std::is_same_v<decltype(I), Zero>) {
                if (!found)
                    zeros++;
            } 
            else {
                found = true;
            }
        });
        int count = 0;
        bool dot = true;
        mp_for_each<mp_reverse<Final>>([&](auto I) {  
            if (count >= zeros && count - zeros < N) {
                printDigit(I);
                if (dot)
                    std::cout << '.';
                    dot = false;
            }
            count++;
        });
        if (count - zeros < N) {
            std::cout << '0';
        }
        if (zeros > 0)
            std::cout << " x 10^-" << zeros << std::endl;
        else 
            std::cout << " x 10^" << tot << std::endl;

    }
};

template <size_t N, typename Final>
int Scientific<N, Final>::zeros = 0;

template <size_t N, typename Final>
bool Scientific<N, Final>::found = false;

template<size_t N, typename Final>
int Scientific<N, Final>::tot = mp_size<Final>::value;

#endif