int main() {
    int data[MAX_LIMIT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    int userIndex;
    std::cout << "Enter index: ";
    std::cin >> userIndex;

    try {
        int value = getArrayValue(data, userIndex);
        std::cout << "Value: " << value << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
