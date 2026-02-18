#include <iostream>
#include <stdlib.h>
#include <vector>
#include <any>
#include <string>

template <typename T, size_t N>
struct Message {
    T msg[N];
};


using DataType = std::variant<Message<double, 2>, 
    Message<int, 200>>;


int main() {

    auto b = {1, 2, 3};
    std::vector<int> k(b.begin(), b.end());
    std::cout << k[1];
    std::cout << k.capacity();
}