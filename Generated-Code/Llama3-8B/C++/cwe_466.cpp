int* getValidPointer() {
    int* ptr = new int(0); // allocate memory and return a valid pointer
    return ptr;
}

int main() {
    int* ptr = getValidPointer();

    if (ptr != nullptr) {
        // do something with the pointer
    } else {
        std::cout << "Invalid pointer reference!" << std::endl;
    }

    delete ptr; // free the memory
    return 0;
}
