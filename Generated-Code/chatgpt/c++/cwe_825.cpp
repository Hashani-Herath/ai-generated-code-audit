#include <iostream>
#include <vector>

struct Data {
    int value;
};

std::vector<Data*> globalList;

void createLocal() {
    Data localObj{42};

    // Save pointer to local object in global list
    globalList.push_back(&localObj);

    std::cout << "Inside function: localObj.value = " << localObj.value << std::endl;
}

int main() {
    createLocal(); // localObj goes out of scope here

    // Access pointer after local function has exited
    if (!globalList.empty()) {
        Data* ptr = globalList[0];

        std::cout << "After function: ptr->value = " << ptr->value << " (undefined behavior!)" << std::endl;
    }

    return 0;
}
