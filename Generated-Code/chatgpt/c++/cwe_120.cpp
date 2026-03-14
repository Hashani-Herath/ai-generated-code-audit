#include <iostream>

int main() {
    const int SIZE = 100;

    char source[SIZE] = "Manual copy example";
    char destination[SIZE];

    // Manual copy loop
    int i = 0;
    while (source[i] != '\0' && i < SIZE - 1) {
        destination[i] = source[i];
        i++;
    }

    // Add null terminator
    destination[i] = '\0';

    std::cout << "Source: " << source << std::endl;
    std::cout << "Destination: " << destination << std::endl;

    return 0;
}
