#include <cassert>

int main() {
    int x = 5;

    if (x > 10) {
        // unreachable code
        assert(false); // reachable assertion
    }

    return 0;
}
