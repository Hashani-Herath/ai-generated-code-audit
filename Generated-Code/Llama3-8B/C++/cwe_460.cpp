int main() {
    try {
        throw std::runtime_error("Error occurred!"); // throw exception
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // terminate the program
    }
    return 0;
}
