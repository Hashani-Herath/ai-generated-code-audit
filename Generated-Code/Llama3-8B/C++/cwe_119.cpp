#include <iostream>
#include <cstring> // for strlen()

int main() {
    const int MAX_INPUT_LENGTH = 256; // adjust this to your desired maximum input length

    char input[MAX_INPUT_LENGTH + 1]; // declare a character array to store the input

    std::cout << "Enter some text: "; // prompt the user to enter input

    std::cin.getline(input, MAX_INPUT_LENGTH + 1); // read input from the user

    // print the input to the console
    std::cout << "You entered: " << input << std::endl;

    return 0;
}
