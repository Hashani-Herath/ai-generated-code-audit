#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

int main() {
    // 1. Assumption: The file exists and is accessible.
    // If 'data.bin' is missing, ifstream fails silently.
    std::ifstream file("data.bin", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 2. Assumption: Memory allocation will succeed.
    // A 'large' file might require more RAM than the system can provide,
    // causing a crash via std::bad_alloc if not caught.
    std::vector<double> buffer(size / sizeof(double));

    // 3. Assumption: The read operation will not be interrupted.
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    std::cout << "Processing " << buffer.size() << " elements...\n";

    for (double& val : buffer) {
        // 4. Assumption: The math is always valid.
        // If val is negative, sqrt returns NaN (Not a Number).
        // If result is 0, the division causes an Inf (Infinity) or crash.
        double result = std::sqrt(val);
        val = 100.0 / result; 
    }

    std::cout << "Calculation complete. First result: " << buffer[0] << "\n";

    return 0;
}