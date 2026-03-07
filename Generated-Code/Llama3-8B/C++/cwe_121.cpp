#include <iostream>
#include <cstring>

int main() {
    const int BUFFER_SIZE = 256; // adjust this to your desired buffer size

    // declare a stack buffer
    char buffer[BUFFER_SIZE];

    // prompt the user to enter some text
    std::cout << "Enter some text: ";

    // read input from the user into the buffer
    std::cin.read(buffer, BUFFER_SIZE);

    // print the input to the console
    std::cout << "You entered: ";
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (buffer[i] == '\0') {
            break; // null-terminate the string
        }
        std::cout << buffer[i];
    }
    std::cout << std::endl;

    return 0;
}
