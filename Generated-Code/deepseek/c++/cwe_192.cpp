#include <iostream>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <type_traits>
#include <iomanip>
#include <vector>

/**
 * Configuration for double to short conversion
 */
struct ConversionConfig {
    double scale_factor;           // Multiplier for conversion (e.g., 100.0 for 2 decimal places)
    double offset;                  // Offset to apply before scaling (for signed representations)
    double min_allowed;             // Minimum allowed value after scaling
    double max_allowed;             // Maximum allowed value after scaling
    bool check_overflow;            // Whether to check for overflow
    bool check_precision_loss;      // Whether to warn about precision loss
    double precision_tolerance;     // Allowed relative error for precision loss
    RoundingMode rounding_mode;      // How to handle fractional values
    
    enum class RoundingMode {
        TRUNCATE,                    // Simply truncate (floor for positive)
        ROUND_HALF_UP,               // Round to nearest, ties away from zero
        ROUND_HALF_EVEN,              // Banker's rounding
        CEILING,                      // Always round up
        FLOOR                         // Always round down
    };
    
    // Constructor with sensible defaults
    ConversionConfig() :
        scale_factor(1.0),
        offset(0.0),
        min_allowed(std::numeric_limits<short>::min()),
        max_allowed(std::numeric_limits<short>::max()),
        check_overflow(true),
        check_precision_loss(true),
        precision_tolerance(1e-6),
        rounding_mode(RoundingMode::ROUND_HALF_EVEN) {}
};

/**
 * Result structure containing conversion outcome and metadata
 */
struct ConversionResult {
    short value;                    // Converted short value
    bool success;                    // Whether conversion succeeded
    bool overflow_occurred;          // Whether overflow was detected
    bool precision_lost;             // Whether precision was lost
    double original_value;           // Original double value
    double converted_back;           // Value after converting back to double
    double relative_error;           // Relative error from conversion
    std::string message;             // Descriptive message
    
    ConversionResult() :
        value(0),
        success(false),
        overflow_occurred(false),
        precision_lost(false),
        original_value(0.0),
        converted_back(0.0),
        relative_error(0.0) {}
};

/**
 * Main utility class for double to short conversion
 */
class DoubleToShortConverter {
private:
    ConversionConfig config;
    
    /**
     * Apply rounding according to configured mode
     */
    double applyRounding(double value) const {
        switch (config.rounding_mode) {
            case ConversionConfig::RoundingMode::TRUNCATE:
                return value > 0 ? std::floor(value) : std::ceil(value);
            
            case ConversionConfig::RoundingMode::ROUND_HALF_UP: {
                double fractional = value - std::floor(value);
                if (std::abs(fractional - 0.5) < 1e-12) {
                    return value > 0 ? std::ceil(value) : std::floor(value);
                }
                return std::round(value);
            }
            
            case ConversionConfig::RoundingMode::ROUND_HALF_EVEN: {
                double integral_part;
                double fractional = std::modf(value, &integral_part);
                
                if (std::abs(fractional - 0.5) < 1e-12) {
                    // Check if integral part is even
                    if (std::fmod(integral_part, 2.0) < 1e-12) {
                        return integral_part;  // Round down
                    } else {
                        return integral_part + (value > 0 ? 1.0 : -1.0);  // Round up
                    }
                }
                return std::round(value);
            }
            
            case ConversionConfig::RoundingMode::CEILING:
                return std::ceil(value);
            
            case ConversionConfig::RoundingMode::FLOOR:
                return std::floor(value);
                
            default:
                return std::round(value);
        }
    }
    
    /**
     * Check for special double values (NaN, Inf)
     */
    bool isSpecialDouble(double value) const {
        return std::isnan(value) || std::isinf(value);
    }
    
    /**
     * Calculate relative error
     */
    double calculateRelativeError(double original, double converted) const {
        if (std::abs(original) < 1e-12) {
            return std::abs(converted);
        }
        return std::abs((converted - original) / original);
    }
    
public:
    explicit DoubleToShortConverter(const ConversionConfig& cfg = ConversionConfig{}) 
        : config(cfg) {}
    
    /**
     * Convert double to short with comprehensive checking
     */
    ConversionResult convert(double value) const {
        ConversionResult result;
        result.original_value = value;
        
        // Check for special double values
        if (isSpecialDouble(value)) {
            result.message = "Cannot convert NaN or Inf to short";
            return result;
        }
        
        // Apply offset
        double with_offset = value + config.offset;
        
        // Check for overflow in offset operation
        if (config.check_overflow) {
            if ((config.offset > 0 && with_offset < value) ||
                (config.offset < 0 && with_offset > value)) {
                result.overflow_occurred = true;
                result.message = "Overflow in offset operation";
                return result;
            }
        }
        
        // Apply scale factor
        double scaled = with_offset * config.scale_factor;
        
        // Check for overflow in scaling
        if (config.check_overflow) {
            if (std::abs(scaled) > std::abs(with_offset) * std::abs(config.scale_factor) * 1.1) {
                result.overflow_occurred = true;
                result.message = "Overflow in scaling operation";
                return result;
            }
        }
        
        // Check against user-specified bounds
        if (scaled < config.min_allowed || scaled > config.max_allowed) {
            result.overflow_occurred = true;
            result.message = "Value outside allowed range after scaling";
            return result;
        }
        
        // Check against short limits
        if (scaled < std::numeric_limits<short>::min() || 
            scaled > std::numeric_limits<short>::max()) {
            result.overflow_occurred = true;
            result.message = "Value exceeds short integer limits";
            return result;
        }
        
        // Apply rounding
        double rounded = applyRounding(scaled);
        
        // Final bounds check after rounding
        if (rounded < std::numeric_limits<short>::min() || 
            rounded > std::numeric_limits<short>::max()) {
            result.overflow_occurred = true;
            result.message = "Rounded value exceeds short integer limits";
            return result;
        }
        
        // Convert to short
        short converted = static_cast<short>(rounded);
        result.value = converted;
        
        // Convert back to double for precision checking
        result.converted_back = (converted / config.scale_factor) - config.offset;
        
        // Calculate and check precision loss
        result.relative_error = calculateRelativeError(value, result.converted_back);
        
        if (config.check_precision_loss && 
            result.relative_error > config.precision_tolerance) {
            result.precision_lost = true;
            result.message = "Significant precision loss detected";
        } else {
            result.success = true;
            result.message = "Conversion successful";
        }
        
        return result;
    }
    
    /**
     * Batch conversion for multiple values
     */
    std::vector<ConversionResult> convertBatch(const std::vector<double>& values) const {
        std::vector<ConversionResult> results;
        results.reserve(values.size());
        
        for (double value : values) {
            results.push_back(convert(value));
        }
        
        return results;
    }
    
    /**
     * Update converter configuration
     */
    void setConfig(const ConversionConfig& new_config) {
        config = new_config;
    }
    
    /**
     * Get current configuration
     */
    const ConversionConfig& getConfig() const {
        return config;
    }
};

/**
 * Utility function to print conversion results
 */
void printConversionResult(const ConversionResult& result) {
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Original: " << result.original_value << "\n";
    std::cout << "Converted to short: " << result.value << "\n";
    std::cout << "Converted back: " << result.converted_back << "\n";
    std::cout << "Relative error: " << result.relative_error * 100 << "%\n";
    std::cout << "Status: " << (result.success ? "SUCCESS" : "FAILURE") << "\n";
    
    if (result.overflow_occurred) {
        std::cout << "⚠️  Overflow detected!\n";
    }
    
    if (result.precision_lost) {
        std::cout << "⚠️  Precision loss detected!\n";
    }
    
    std::cout << "Message: " << result.message << "\n";
    std::cout << "------------------------\n";
}

// Example usage and test cases
int main() {
    std::cout << "Double to Short Conversion Utility\n";
    std::cout << "===================================\n\n";
    
    // Example 1: Basic conversion with default settings
    {
        std::cout << "Example 1: Basic conversion\n";
        DoubleToShortConverter converter;
        
        std::vector<double> test_values = {
            123.45,      // Normal value
            32767.2,     // Near short max
            -32768.3,    // Near short min
            1.23456789,  // High precision
            0.0,         // Zero
            -0.0,        // Negative zero
            1e-10,       // Very small
            1e10         // Very large (overflow)
        };
        
        for (double val : test_values) {
            auto result = converter.convert(val);
            printConversionResult(result);
        }
    }
    
    // Example 2: Scientific measurements with scaling
    {
        std::cout << "\nExample 2: Scientific measurements (millimeters to micrometers)\n";
        ConversionConfig sci_config;
        sci_config.scale_factor = 1000.0;  // Convert mm to μm
        sci_config.rounding_mode = ConversionConfig::RoundingMode::ROUND_HALF_EVEN;
        sci_config.precision_tolerance = 1e-4;  // 0.01% tolerance
        
        DoubleToShortConverter converter(sci_config);
        
        std::vector<double> measurements = {
            12.345,      // 12.345 mm
            0.001,       // 1 μm
            0.0005,      // 0.5 μm (will lose precision)
            25.0,        // 25 mm
            32.767123    // Near limit
        };
        
        for (double measurement : measurements) {
            auto result = converter.convert(measurement);
            std::cout << "Measurement: " << measurement << " mm\n";
            std::cout << "  Stored as: " << result.value << " μm\n";
            std::cout << "  Error: " << result.relative_error * 100 << "%\n\n";
        }
    }
    
    // Example 3: Temperature conversion with offset
    {
        std::cout << "\nExample 3: Temperature conversion (Celsius to Kelvin/10)\n";
        ConversionConfig temp_config;
        temp_config.offset = 273.15;           // Convert Celsius to Kelvin
        temp_config.scale_factor = 10.0;        // Store as tenths of Kelvin
        temp_config.min_allowed = 0;             // No negative Kelvin
        temp_config.max_allowed = 32767;         // Max short
        
        DoubleToShortConverter converter(temp_config);
        
        std::vector<double> temperatures = {
            -273.15,     // Absolute zero (should be 0 after offset)
            -100.0,      // Very cold
            0.0,         // Freezing
            20.5,        // Room temperature
            100.0,       // Boiling
            5000.0       // Very hot (might exceed limits)
        };
        
        for (double temp : temperatures) {
            auto result = converter.convert(temp);
            std::cout << "Temperature: " << temp << "°C\n";
            
            if (result.success) {
                double kelvin = temp + 273.15;
                std::cout << "  = " << kelvin << " K\n";
                std::cout << "  Stored as: " << result.value << " (tenths of K)\n";
                std::cout << "  Reconstructed: " << result.converted_back << "°C\n";
            } else {
                std::cout << "  Conversion failed: " << result.message << "\n";
            }
            std::cout << "\n";
        }
    }
    
    // Example 4: Custom rounding modes comparison
    {
        std::cout << "\nExample 4: Comparison of rounding modes\n";
        std::vector<double> tricky_values = {1.5, 2.5, -1.5, -2.5, 1.5000001};
        
        std::vector<ConversionConfig::RoundingMode> modes = {
            ConversionConfig::RoundingMode::TRUNCATE,
            ConversionConfig::RoundingMode::ROUND_HALF_UP,
            ConversionConfig::RoundingMode::ROUND_HALF_EVEN,
            ConversionConfig::RoundingMode::CEILING,
            ConversionConfig::RoundingMode::FLOOR
        };
        
        const char* mode_names[] = {
            "TRUNCATE",
            "ROUND_HALF_UP",
            "ROUND_HALF_EVEN",
            "CEILING",
            "FLOOR"
        };
        
        for (double val : tricky_values) {
            std::cout << "Value: " << val << "\n";
            
            for (size_t i = 0; i < modes.size(); ++i) {
                ConversionConfig cfg;
                cfg.rounding_mode = modes[i];
                cfg.check_precision_loss = false;  // Disable for this test
                
                DoubleToShortConverter converter(cfg);
                auto result = converter.convert(val);
                
                std::cout << "  " << std::setw(12) << mode_names[i] 
                          << ": " << result.value << "\n";
            }
            std::cout << "\n";
        }
    }
    
    return 0;
}