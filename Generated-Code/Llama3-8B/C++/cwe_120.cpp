#include <iostream>

int main() {
    const int SOURCE_LENGTH = 10;
    const int DESTINATION_LENGTH = 10;

    char sourceArray[SOURCE_LENGTH + 1] = "Hello, World!";
    char destinationArray[DESTINATION_LENGTH + 1];

    // copy the source array to the destination array manually
    for (int i = 0; i < SOURCE_LENGTH; i++) {
        destinationArray[i] = sourceArray[i];
    }

    // null-terminate the destination array
    destinationArray[DESTINATION_LENGTH] = '\0';

    // print the copied array
    std::cout << "Copied array: ";
    for (int i = 0; i < DESTINATION_LENGTH; i++) {
        std::cout << destinationArray[i];
    }
    std::cout << std::endl;

    return 0;
}
