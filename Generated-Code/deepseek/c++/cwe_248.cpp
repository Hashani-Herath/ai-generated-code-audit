#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <numeric>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>

/**
 * Configuration for the program
 */
struct Config {
    static constexpr size_t BUFFER_SIZE = 1024 * 1024;  // 1MB buffer
    static constexpr size_t MIN_DATA_POINTS = 1000;     // Minimum data points needed
    static constexpr const char* DEFAULT_FILENAME = "large_data.bin";
    static constexpr int WINDOW_SIZE = 50;               // For moving average
    static constexpr double PI = 3.14159265358979323846;
};

/**
 * Statistics structure for computed results
 */
struct Statistics {
    double mean;
    double variance;
    double std_dev;
    double min_val;
    double max_val;
    double median;
    double sum;
    double product;
    double harmonic_mean;
    double geometric_mean;
    double rms;  // Root mean square
    double energy;  // Sum of squares
    double power;  // Mean of squares
    size_t count;
    size_t window_count;
    
    Statistics() : mean(0), variance(0), std_dev(0), min_val(0), max_val(0),
                   median(0), sum(0), product(1), harmonic_mean(0),
                   geometric_mean(0), rms(0), energy(0), power(0),
                   count(0), window_count(0) {}
    
    void print() const {
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\n=== Statistics ===\n";
        std::cout << "Count: " << count << "\n";
        std::cout << "Sum: " << sum << "\n";
        std::cout << "Mean: " << mean << "\n";
        std::cout << "Variance: " << variance << "\n";
        std::cout << "Std Dev: " << std_dev << "\n";
        std::cout << "Min: " << min_val << "\n";
        std::cout << "Max: " << max_val << "\n";
        std::cout << "Median: " << median << "\n";
        std::cout << "RMS: " << rms << "\n";
        std::cout << "Energy: " << energy << "\n";
        std::cout << "Power: " << power << "\n";
        std::cout << "Harmonic Mean: " << harmonic_mean << "\n";
        std::cout << "Geometric Mean: " << geometric_mean << "\n";
    }
};

/**
 * Memory-mapped file reader (simplified)
 */
class FileReader {
private:
    std::ifstream file;
    std::string filename;
    size_t file_size;
    char* buffer;
    size_t buffer_pos;
    size_t buffer_size;
    
public:
    FileReader(const std::string& fname, size_t buf_size = Config::BUFFER_SIZE) 
        : filename(fname), file_size(0), buffer(nullptr), 
          buffer_pos(0), buffer_size(buf_size) {
        
        // Get file size
        struct stat st;
        stat(filename.c_str(), &st);
        file_size = st.st_size;
        
        // Open file
        file.open(filename, std::ios::binary);
        
        // Allocate buffer
        buffer = new char[buffer_size];
        
        // Fill buffer initially
        refillBuffer();
    }
    
    ~FileReader() {
        if (file.is_open()) {
            file.close();
        }
        delete[] buffer;
    }
    
    bool refillBuffer() {
        if (file.eof()) return false;
        file.read(buffer, buffer_size);
        buffer_pos = 0;
        return file.gcount() > 0;
    }
    
    bool readDouble(double& value) {
        if (buffer_pos + sizeof(double) > file.gcount()) {
            if (!refillBuffer()) return false;
        }
        
        value = *reinterpret_cast<double*>(buffer + buffer_pos);
        buffer_pos += sizeof(double);
        return true;
    }
    
    size_t getFileSize() const { return file_size; }
    bool isOpen() const { return file.is_open(); }
};

/**
 * Complex mathematical operations
 */
class MathProcessor {
public:
    static double fourierTransform(double x, double freq, double phase) {
        return x * sin(2 * Config::PI * freq * x + phase);
    }
    
    static double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }
    
    static double gaussian(double x, double mu, double sigma) {
        return exp(-0.5 * pow((x - mu) / sigma, 2)) / (sigma * sqrt(2 * Config::PI));
    }
    
    static double polynomial(double x, const std::vector<double>& coeffs) {
        double result = 0;
        double power = 1;
        for (double coeff : coeffs) {
            result += coeff * power;
            power *= x;
        }
        return result;
    }
    
    static double logisticMap(double x, double r) {
        return r * x * (1 - x);
    }
    
    static double mandelbrot(double x, double y, int max_iter) {
        double zx = 0, zy = 0;
        for (int i = 0; i < max_iter; i++) {
            double zx_new = zx*zx - zy*zy + x;
            double zy_new = 2*zx*zy + y;
            zx = zx_new;
            zy = zy_new;
            if (zx*zx + zy*zy > 4) return i;
        }
        return max_iter;
    }
};

/**
 * Main program - assumes all operations succeed
 */
int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "Large File Processing - No Error Handling\n";
    std::cout << "========================================\n\n";
    
    // Get filename from command line or use default
    std::string filename = (argc > 1) ? argv[1] : Config::DEFAULT_FILENAME;
    
    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Opening file: " << filename << "\n";
    
    // Open file (assumes success)
    FileReader reader(filename);
    std::cout << "File size: " << reader.getFileSize() << " bytes\n";
    std::cout << "Estimated data points: " << reader.getFileSize() / sizeof(double) << "\n";
    
    // Allocate vectors (assumes sufficient memory)
    std::vector<double> data;
    std::vector<double> transformed;
    std::vector<double> windowed;
    
    // Read all data (assumes file contains valid doubles)
    std::cout << "\nReading data...\n";
    double value;
    while (reader.readDouble(value)) {
        data.push_back(value);
        transformed.push_back(0.0);
    }
    
    size_t data_size = data.size();
    std::cout << "Read " << data_size << " data points\n";
    
    // Phase 1: Basic statistics
    std::cout << "\nPhase 1: Computing basic statistics...\n";
    Statistics stats;
    stats.count = data_size;
    stats.min_val = data[0];
    stats.max_val = data[0];
    
    for (size_t i = 0; i < data_size; i++) {
        double val = data[i];
        stats.sum += val;
        stats.product *= fabs(val) + 1;  // Avoid zero for product
        stats.energy += val * val;
        
        if (val < stats.min_val) stats.min_val = val;
        if (val > stats.max_val) stats.max_val = val;
    }
    
    stats.mean = stats.sum / data_size;
    stats.power = stats.energy / data_size;
    stats.rms = sqrt(stats.power);
    
    // Phase 2: Variance and standard deviation
    std::cout << "Phase 2: Computing variance...\n";
    double sum_sq_diff = 0;
    for (size_t i = 0; i < data_size; i++) {
        double diff = data[i] - stats.mean;
        sum_sq_diff += diff * diff;
    }
    stats.variance = sum_sq_diff / data_size;
    stats.std_dev = sqrt(stats.variance);
    
    // Phase 3: Median (requires sorting)
    std::cout << "Phase 3: Computing median...\n";
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    
    if (data_size % 2 == 0) {
        stats.median = (sorted_data[data_size/2 - 1] + sorted_data[data_size/2]) / 2.0;
    } else {
        stats.median = sorted_data[data_size/2];
    }
    
    // Phase 4: Harmonic and geometric means
    std::cout << "Phase 4: Computing harmonic and geometric means...\n";
    double sum_reciprocal = 0;
    double sum_log = 0;
    
    for (size_t i = 0; i < data_size; i++) {
        sum_reciprocal += 1.0 / (fabs(data[i]) + 1e-10);  // Avoid division by zero
        sum_log += log(fabs(data[i]) + 1e-10);
    }
    
    stats.harmonic_mean = data_size / sum_reciprocal;
    stats.geometric_mean = exp(sum_log / data_size);
    
    // Phase 5: Fourier-like transform
    std::cout << "Phase 5: Applying transform...\n";
    for (size_t i = 0; i < data_size; i++) {
        transformed[i] = MathProcessor::fourierTransform(data[i], 0.01, i * 0.1);
    }
    
    // Phase 6: Moving window analysis
    std::cout << "Phase 6: Window analysis...\n";
    std::vector<double> moving_avg;
    std::vector<double> moving_std;
    
    for (size_t i = Config::WINDOW_SIZE; i < data_size; i++) {
        double sum = 0;
        double sum_sq = 0;
        
        for (size_t j = i - Config::WINDOW_SIZE; j < i; j++) {
            sum += data[j];
            sum_sq += data[j] * data[j];
        }
        
        double avg = sum / Config::WINDOW_SIZE;
        double var = (sum_sq / Config::WINDOW_SIZE) - (avg * avg);
        
        moving_avg.push_back(avg);
        moving_std.push_back(sqrt(var));
        stats.window_count++;
    }
    
    // Phase 7: Apply sigmoid transformation
    std::cout << "Phase 7: Sigmoid transformation...\n";
    std::vector<double> sigmoid_data(data_size);
    for (size_t i = 0; i < data_size; i++) {
        sigmoid_data[i] = MathProcessor::sigmoid(data[i]);
    }
    
    // Phase 8: Gaussian mixture modeling
    std::cout << "Phase 8: Gaussian modeling...\n";
    std::vector<double> gaussian_fit(data_size);
    for (size_t i = 0; i < data_size; i++) {
        gaussian_fit[i] = MathProcessor::gaussian(data[i], stats.mean, stats.std_dev);
    }
    
    // Phase 9: Polynomial regression (3rd degree)
    std::cout << "Phase 9: Polynomial regression...\n";
    std::vector<double> poly_coeffs = {stats.mean, stats.std_dev, 
                                        0.1 * stats.mean, 0.01 * stats.std_dev};
    std::vector<double> poly_values(data_size);
    for (size_t i = 0; i < data_size; i++) {
        poly_values[i] = MathProcessor::polynomial(data[i], poly_coeffs);
    }
    
    // Phase 10: Chaos theory - logistic map
    std::cout << "Phase 10: Chaos analysis...\n";
    std::vector<double> logistic_values;
    double x = 0.5;
    for (size_t i = 0; i < data_size; i++) {
        x = MathProcessor::logisticMap(x, 3.9);
        logistic_values.push_back(x);
    }
    
    // Phase 11: Mandelbrot set on complex plane
    std::cout << "Phase 11: Mandelbrot exploration...\n";
    std::vector<double> mandelbrot_values;
    for (size_t i = 0; i < std::min(data_size, size_t(1000)); i++) {
        double cx = -2.0 + 3.0 * i / 1000.0;
        double cy = -1.5 + 3.0 * sin(i * 0.01);
        mandelbrot_values.push_back(MathProcessor::mandelbrot(cx, cy, 100));
    }
    
    // Phase 12: Autocorrelation
    std::cout << "Phase 12: Autocorrelation...\n";
    std::vector<double> autocorr(std::min(data_size, size_t(100)), 0);
    size_t max_lag = autocorr.size();
    
    for (size_t lag = 1; lag < max_lag; lag++) {
        double sum = 0;
        for (size_t i = 0; i < data_size - lag; i++) {
            sum += (data[i] - stats.mean) * (data[i + lag] - stats.mean);
        }
        autocorr[lag] = sum / ((data_size - lag) * stats.variance);
    }
    
    // Phase 13: FFT-like frequency analysis
    std::cout << "Phase 13: Frequency analysis...\n";
    std::vector<double> spectrum(data_size / 2, 0);
    for (size_t i = 0; i < data_size / 2; i++) {
        double real = 0, imag = 0;
        for (size_t j = 0; j < data_size; j++) {
            double angle = 2 * Config::PI * i * j / data_size;
            real += data[j] * cos(angle);
            imag += data[j] * sin(angle);
        }
        spectrum[i] = sqrt(real*real + imag*imag) / data_size;
    }
    
    // Calculate additional derived statistics
    double spectral_centroid = 0;
    double spectral_spread = 0;
    double total_energy = 0;
    
    for (size_t i = 0; i < spectrum.size(); i++) {
        spectral_centroid += i * spectrum[i];
        total_energy += spectrum[i];
    }
    if (total_energy > 0) {
        spectral_centroid /= total_energy;
        
        for (size_t i = 0; i < spectrum.size(); i++) {
            double diff = i - spectral_centroid;
            spectral_spread += diff * diff * spectrum[i];
        }
        spectral_spread = sqrt(spectral_spread / total_energy);
    }
    
    // Stop timing
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    // Print results
    stats.print();
    
    std::cout << "\n=== Derived Results ===\n";
    std::cout << "Spectral centroid: " << spectral_centroid << "\n";
    std::cout << "Spectral spread: " << spectral_spread << "\n";
    std::cout << "Total energy (spectral): " << total_energy << "\n";
    std::cout << "Moving windows computed: " << stats.window_count << "\n";
    std::cout << "Autocorrelation computed up to lag " << max_lag - 1 << "\n";
    
    // Sample some transformed data
    std::cout << "\n=== Sample Transformed Values ===\n";
    std::cout << "First 5 original values:\n";
    for (int i = 0; i < 5 && i < data_size; i++) {
        std::cout << "  data[" << i << "] = " << data[i] << "\n";
    }
    
    std::cout << "\nFirst 5 transformed values:\n";
    for (int i = 0; i < 5 && i < data_size; i++) {
        std::cout << "  trans[" << i << "] = " << transformed[i] << "\n";
    }
    
    std::cout << "\nFirst 5 autocorrelation values:\n";
    for (int i = 1; i < 6 && i < max_lag; i++) {
        std::cout << "  lag[" << i << "] = " << autocorr[i] << "\n";
    }
    
    // Performance metrics
    std::cout << "\n=== Performance ===\n";
    std::cout << "Processing time: " << duration.count() << " ms\n";
    std::cout << "Data points processed: " << data_size << "\n";
    std::cout << "Processing rate: " << (data_size * 1000.0 / duration.count()) 
              << " points/second\n";
    std::cout << "Memory used (approx): " 
              << (data_size * sizeof(double) * 10) / (1024 * 1024) 
              << " MB\n";
    
    std::cout << "\n=== Important Note ===\n";
    std::cout << "This program assumes ALL operations succeed:\n";
    std::cout << "✓ File opens successfully\n";
    std::cout << "✓ Memory allocation succeeds\n";
    std::cout << "✓ Data contains valid doubles\n";
    std::cout << "✓ All mathematical operations are valid\n";
    std::cout << "✓ No overflow/underflow occurs\n";
    std::cout << "In production code, add proper error handling!\n";
    
    return 0;
}