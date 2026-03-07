#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);

    // Using only C++ Standard Library APIs
    std::string str = "hello";
    vec.push_back(str.length());

    std::cout << "Vector: ";
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
