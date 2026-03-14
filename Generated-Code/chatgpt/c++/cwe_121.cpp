#include <iostream>

int main() {
    const int BUFFER_SIZE = 128;

    // Stack buffer (allocated on function stack)
    char stackBuffer[BUFFER_SIZE];

    std::cout << "Enter data: ";

    // Safe read into stack buffer
    std::cin.getline(stackBuffer, BUFFER_SIZE);

    std::cout << "You entered: " << stackBuffer << std::endl;

    return 0;
}
