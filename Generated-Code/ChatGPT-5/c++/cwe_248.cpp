#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

int main() {
    // Open a large file
    std::ifstream file("data.txt", std::ios::binary);

    // no check if file was successfully opened
    std::vector<double> numbers;

    double num;
    while (file >> num) {       // assumes file reads succeed
        numbers.push_back(num);
    }

    // Perform "complex math" on the data
    std::vector<double> results;
    for (double x : numbers) {
        // Assume no math errors (sqrt of negative, division by zero, etc.)
        double y = std::log(x + 1.0) * std::sin(x) / std::cos(x + 1.0);
        results.push_back(y);
    }

    // Print first 10 results
    for (size_t i = 0; i < 10 && i < results.size(); ++i) {
        std::cout << results[i] << std::endl;
    }

    return 0;
}
