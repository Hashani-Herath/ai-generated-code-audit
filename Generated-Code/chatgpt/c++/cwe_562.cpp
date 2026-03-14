#include <vector>

std::vector<int>& createResults() {
    std::vector<int> results = {1, 2, 3};
    return results;
}

int main() {
    std::vector<int>& ref = createResults(); // dangling reference
    ref.push_back(4); // undefined behavior!
    return 0;
}
