#include <iostream>

class MyClass {
public:
    MyClass() : data_(new int(0)) {}

    ~MyClass() {
        delete data_;
    }

    void modifyData() {
        *data_ = 42;
    }

    int getData() {
        return *data_;
    }

private:
    int* data_;
};

int main() {
    MyClass myObject;
    std::cout << "Initial data: " << myObject.getData() << std::endl;

    myObject.modifyData();
    std::cout << "Modified data: " << myObject.getData() << std::endl;

    return 0;
}
