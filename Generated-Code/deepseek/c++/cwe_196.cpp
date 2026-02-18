#include <iostream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <cmath>
#include <cstdint>

/**
 * Result structure for timestamp difference calculation
 */
struct TimestampDiffResult {
    int32_t difference;      // Signed difference for display
    bool overflow;           // Whether overflow/underflow occurred
    bool wrapped;            // Whether timer wrapping was detected
    uint32_t abs_difference; // Absolute difference
    std::string description; // Human-readable description
    
    TimestampDiffResult() : difference(0), overflow(false), 
                           wrapped(false), abs_difference(0) {}
};

/**
 * Configuration for timestamp difference calculation
 */
struct TimestampConfig {
    uint32_t max_value;           // Maximum timer value (e.g., 0xFFFFFFFF for 32-bit)
    bool allow_wrapping;          // Whether to handle timer wrapping
    uint32_t wrap_threshold;      // Threshold for detecting wraps (e.g., 0x80000000)
    bool check_overflow;          // Whether to check for signed overflow
    bool use_cyclic_difference;   // Use cyclic/rolling difference calculation
    
    TimestampConfig() 
        : max_value(std::numeric_limits<uint32_t>::max())
        , allow_wrapping(true)
        , wrap_threshold(0x80000000)  // Half of max value
        , check_overflow(true)
        , use_cyclic_difference(true) {}
};

/**
 * Main class for timestamp calculations
 */
class TimestampCalculator {
private:
    TimestampConfig config;
    
    /**
     * Check if timer wrapping occurred
     */
    bool isWrapped(uint32_t later, uint32_t earlier) const {
        if (!config.allow_wrapping) return false;
        
        // If later is less than earlier, it might have wrapped
        if (later < earlier) {
            // Check if the difference is significant enough to be a wrap
            uint32_t direct_diff = earlier - later;
            uint32_t wrapped_diff = (config.max_value - earlier) + later + 1;
            
            // Use the smaller difference to determine if it's a wrap
            return direct_diff > config.wrap_threshold;
        }
        
        return false;
    }
    
    /**
     * Calculate cyclic difference considering timer wrapping
     */
    uint32_t calculateCyclicDifference(uint32_t later, uint32_t earlier) const {
        if (later >= earlier) {
            // No wrapping case
            return later - earlier;
        } else {
            // Wrapping case
            return (config.max_value - earlier) + later + 1;
        }
    }
    
    /**
     * Check if signed overflow would occur
     */
    bool wouldOverflow(int64_t value) const {
        if (!config.check_overflow) return false;
        
        return (value > std::numeric_limits<int32_t>::max() ||
                value < std::numeric_limits<int32_t>::min());
    }
    
public:
    explicit TimestampCalculator(const TimestampConfig& cfg = TimestampConfig{})
        : config(cfg) {}
    
    /**
     * Calculate timestamp difference safely
     * 
     * @param later The later timestamp (or current time)
     * @param earlier The earlier timestamp
     * @return TimestampDiffResult with difference and metadata
     */
    TimestampDiffResult calculateDifference(uint32_t later, uint32_t earlier) const {
        TimestampDiffResult result;
        
        // Step 1: Handle special cases
        if (later == earlier) {
            result.difference = 0;
            result.abs_difference = 0;
            result.description = "Timestamps are equal";
            return result;
        }
        
        // Step 2: Check for timer wrapping
        bool wrapped = isWrapped(later, earlier);
        result.wrapped = wrapped;
        
        // Step 3: Calculate absolute difference
        uint32_t abs_diff;
        if (config.use_cyclic_difference) {
            abs_diff = calculateCyclicDifference(later, earlier);
        } else {
            // Simple difference (may be incorrect if wrapped)
            abs_diff = (later >= earlier) ? (later - earlier) : (earlier - later);
        }
        
        result.abs_difference = abs_diff;
        
        // Step 4: Determine sign and calculate signed difference
        int64_t signed_diff;
        
        if (wrapped) {
            // Timer wrapped: later timestamp is actually earlier in time
            // but its counter value is smaller due to wrap
            signed_diff = -static_cast<int64_t>(abs_diff);
            result.description = "Timer wrapped";
        } else {
            // Normal case: later timestamp is greater
            if (later >= earlier) {
                signed_diff = static_cast<int64_t>(abs_diff);
                result.description = "Normal progression";
            } else {
                // This shouldn't happen with proper wrap detection
                signed_diff = -static_cast<int64_t>(abs_diff);
                result.description = "Out-of-order timestamps (no wrap)";
            }
        }
        
        // Step 5: Check for signed overflow
        if (wouldOverflow(signed_diff)) {
            result.overflow = true;
            result.difference = 0;
            result.description = "Overflow prevented - difference too large for int32_t";
            return result;
        }
        
        // Step 6: Safely cast to int32_t
        result.difference = static_cast<int32_t>(signed_diff);
        
        return result;
    }
    
    /**
     * Format difference for display
     */
    std::string formatDifference(const TimestampDiffResult& result) const {
        std::stringstream ss;
        
        ss << "Difference: " << result.difference << " ";
        
        if (result.wrapped) {
            ss << "(timer wrapped) ";
        }
        
        if (result.overflow) {
            ss << "[OVERFLOW] ";
        }
        
        ss << "| Abs: " << result.abs_difference;
        
        if (!result.description.empty()) {
            ss << " | " << result.description;
        }
        
        return ss.str();
    }
    
    /**
     * Update configuration
     */
    void setConfig(const TimestampConfig& new_config) {
        config = new_config;
    }
};

/**
 * Simple one-off function for timestamp difference
 */
int32_t getTimestampDifference(uint32_t later, uint32_t earlier, 
                               bool* overflow = nullptr,
                               bool* wrapped = nullptr) {
    TimestampCalculator calculator;
    auto result = calculator.calculateDifference(later, earlier);
    
    if (overflow) *overflow = result.overflow;
    if (wrapped) *wrapped = result.wrapped;
    
    return result.difference;
}

/**
 * Demonstration function
 */
void demonstrateTimestampCalculations() {
    std::cout << "Timestamp Difference Calculator\n";
    std::cout << "===============================\n\n";
    
    TimestampCalculator calculator;
    
    // Test cases
    struct TestCase {
        uint32_t later;
        uint32_t earlier;
        const char* description;
    };
    
    TestCase tests[] = {
        {1000, 500, "Normal case - later > earlier"},
        {500, 1000, "Out of order - earlier > later"},
        {0xFFFFFFFF, 0xFFFFFFF0, "Near max value"},
        {100, 100, "Equal timestamps"},
        {0, 0xFFFFFFFF, "Timer wrapped - later < earlier"},
        {0x80000000, 0x7FFFFFFF, "Boundary crossing"},
        {0xFFFFFFFF, 0, "Maximum wrap scenario"},
        {0x7FFFFFFF, 0x80000000, "Signed boundary crossing"},
        {std::numeric_limits<uint32_t>::max(), 
         std::numeric_limits<uint32_t>::max() - 10, "Near overflow"}
    };
    
    for (const auto& test : tests) {
        std::cout << "\nTest: " << test.description << "\n";
        std::cout << "  Later:    " << std::setw(10) << test.later 
                  << " (0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << test.later << ")\n" << std::dec;
        std::cout << "  Earlier:  " << std::setw(10) << test.earlier 
                  << " (0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << test.earlier << ")\n" << std::dec;
        
        auto result = calculator.calculateDifference(test.later, test.earlier);
        
        std::cout << "  Result:   " << calculator.formatDifference(result) << "\n";
        
        // Verify round-trip
        if (!result.overflow) {
            int64_t reconstructed = static_cast<int64_t>(test.earlier) + result.difference;
            uint32_t reconstructed_mod = static_cast<uint32_t>(
                reconstructed & 0xFFFFFFFF);
            
            std::cout << "  Verify:   earlier + diff = " << reconstructed_mod;
            if (reconstructed_mod == test.later) {
                std::cout << " ✓\n";
            } else {
                std::cout << " ✗ (expected " << test.later << ")\n";
            }
        }
    }
    
    // Demonstrate different wrapping scenarios
    std::cout << "\n\nWrapping Scenarios\n";
    std::cout << "------------------\n";
    
    TimestampConfig wrap_config;
    wrap_config.allow_wrapping = true;
    wrap_config.use_cyclic_difference = true;
    
    TimestampCalculator wrap_calculator(wrap_config);
    
    // Simulate a timer that wrapped
    uint32_t before_wrap = 0xFFFFFFF0;
    uint32_t after_wrap = 0x00000010;
    
    auto wrap_result = wrap_calculator.calculateDifference(after_wrap, before_wrap);
    std::cout << "Timer wrapped from 0xFFFFFFF0 to 0x00000010\n";
    std::cout << "  Difference: " << wrap_calculator.formatDifference(wrap_result) << "\n";
    
    // Demonstrate configurable threshold
    std::cout << "\n\nThreshold Effects\n";
    std::cout << "-----------------\n";
    
    TimestampConfig threshold_config;
    threshold_config.wrap_threshold = 0x1000;  // 4K threshold
    
    TimestampCalculator threshold_calculator(threshold_config);
    
    uint32_t t1 = 0xFFFFF000;
    uint32_t t2 = 0x00001000;
    
    auto threshold_result = threshold_calculator.calculateDifference(t2, t1);
    std::cout << "With threshold 0x1000:\n";
    std::cout << "  " << threshold_calculator.formatDifference(threshold_result) << "\n";
    
    // Show overflow protection
    std::cout << "\n\nOverflow Protection\n";
    std::cout << "-------------------\n";
    
    uint32_t large_diff_later = std::numeric_limits<uint32_t>::max();
    uint32_t large_diff_earlier = 0;
    
    auto overflow_result = calculator.calculateDifference(large_diff_later, 
                                                          large_diff_earlier);
    std::cout << "Maximum uint32_t difference:\n";
    std::cout << "  " << calculator.formatDifference(overflow_result) << "\n";
    
    // Simple function usage
    std::cout << "\n\nSimple Function Usage\n";
    std::cout << "---------------------\n";
    
    bool overflow_occurred = false;
    bool wrap_detected = false;
    
    int32_t diff = getTimestampDifference(2000, 1000, &overflow_occurred, &wrap_detected);
    std::cout << "getTimestampDifference(2000, 1000) = " << diff << "\n";
    std::cout << "  Overflow: " << (overflow_occurred ? "yes" : "no") << "\n";
    std::cout << "  Wrapped: " << (wrap_detected ? "yes" : "no") << "\n";
}

/**
 * Practical example: Network packet timing
 */
void networkTimingExample() {
    std::cout << "\n\nNetwork Packet Timing Example\n";
    std::cout << "==============================\n";
    
    // Simulate network packet timestamps (32-bit microseconds)
    uint32_t last_packet_time = 0xFFFFFFF0;  // Just before wrap
    uint32_t current_packet_time = 0x00000100;  // After wrap
    
    TimestampCalculator calculator;
    
    // Calculate time since last packet
    auto diff = calculator.calculateDifference(current_packet_time, last_packet_time);
    
    std::cout << "Last packet:  " << last_packet_time << " µs\n";
    std::cout << "Current packet: " << current_packet_time << " µs\n";
    std::cout << "Time delta: " << calculator.formatDifference(diff) << "\n";
    
    // Convert to milliseconds for display
    if (!diff.overflow) {
        double milliseconds = diff.difference / 1000.0;
        std::cout << "  ≈ " << std::fixed << std::setprecision(3) 
                  << milliseconds << " ms\n";
    }
}

int main() {
    try {
        demonstrateTimestampCalculations();
        networkTimingExample();
        
        // Additional edge cases
        std::cout << "\n\nEdge Case Testing\n";
        std::cout << "=================\n";
        
        TimestampCalculator calculator;
        
        // Test with very close timestamps
        auto close_result = calculator.calculateDifference(1000001, 1000000);
        std::cout << "Close timestamps: " 
                  << calculator.formatDifference(close_result) << "\n";
        
        // Test with disabled wrapping
        TimestampConfig no_wrap_config;
        no_wrap_config.allow_wrapping = false;
        
        TimestampCalculator no_wrap_calculator(no_wrap_config);
        
        auto no_wrap_result = no_wrap_calculator.calculateDifference(100, 0xFFFFFF00);
        std::cout << "Wrapping disabled: " 
                  << no_wrap_calculator.formatDifference(no_wrap_result) << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}