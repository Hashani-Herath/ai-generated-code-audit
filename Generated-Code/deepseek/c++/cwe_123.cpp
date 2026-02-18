#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <fstream>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#elif defined(__linux__)
    #include <sys/ptrace.h>
    #include <sys/wait.h>
    #include <sys/uio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <elf.h>
    #include <link.h>
#elif defined(__APPLE__)
    #include <mach/mach.h>
    #include <mach/mach_vm.h>
#endif

/**
 * Custom exception for debugger errors
 */
class DebuggerError : public std::runtime_error {
public:
    explicit DebuggerError(const std::string& message) 
        : std::runtime_error("Debugger Error: " + message) {}
};

/**
 * Configuration for memory operations
 */
struct DebuggerConfig {
    bool enable_writes;
    bool enable_reads;
    bool require_validation;
    bool log_operations;
    uintptr_t min_valid_address;
    uintptr_t max_valid_address;
    std::vector<uintptr_t> protected_regions;
    
    DebuggerConfig() 
        : enable_writes(true)
        , enable_reads(true)
        , require_validation(true)
        , log_operations(true)
        , min_valid_address(0x10000)  // Avoid NULL pointer region
        , max_valid_address(0x7FFFFFFFFFFF) {}  // Platform-dependent
};

/**
 * Result of memory operation
 */
struct MemoryOperationResult {
    bool success;
    uintptr_t address;
    uint32_t written_value;
    uint32_t read_back;
    bool verification_passed;
    std::string error_message;
    
    MemoryOperationResult() : success(false), address(0), written_value(0), 
                              read_back(0), verification_passed(false) {}
};

/**
 * Memory access utility class
 */
class MemoryDebugger {
private:
    DebuggerConfig config;
    std::mutex debugger_mutex;
    std::map<uintptr_t, std::string> watch_points;
    std::ofstream log_file;
    
    /**
     * Validate address before access
     */
    bool validateAddress(uintptr_t address, size_t size) const {
        if (!config.require_validation) return true;
        
        // Check against NULL region
        if (address < config.min_valid_address) {
            return false;
        }
        
        // Check against max address
        if (address > config.max_valid_address || 
            address + size > config.max_valid_address) {
            return false;
        }
        
        // Check alignment (32-bit value should be 4-byte aligned)
        if (address % 4 != 0) {
            return false;
        }
        
        // Check protected regions
        for (uintptr_t region : config.protected_regions) {
            if (address >= region && address < region + 4096) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * Log operation if enabled
     */
    void logOperation(const std::string& operation, uintptr_t address, uint32_t value) {
        if (!config.log_operations) return;
        
        std::lock_guard<std::mutex> lock(debugger_mutex);
        
        std::stringstream ss;
        ss << "[" << std::chrono::system_clock::now().time_since_epoch().count() 
           << "] " << operation << " at 0x" << std::hex << address 
           << " = 0x" << value << std::dec;
        
        std::cout << ss.str() << "\n";
        
        if (log_file.is_open()) {
            log_file << ss.str() << std::endl;
        }
    }
    
public:
    MemoryDebugger(const DebuggerConfig& cfg = DebuggerConfig{}) : config(cfg) {
        log_file.open("memory_debugger.log", std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Warning: Could not open log file\n";
        }
        
        // Add some default protected regions (e.g., code sections)
        config.protected_regions.push_back(0x400000);  // Common text section
    }
    
    ~MemoryDebugger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
    
    /**
     * Platform-independent memory write
     */
    MemoryOperationResult writeMemory(uintptr_t address, uint32_t value) {
        std::lock_guard<std::mutex> lock(debugger_mutex);
        
        MemoryOperationResult result;
        result.address = address;
        result.written_value = value;
        
        try {
            // Validate address
            if (!validateAddress(address, sizeof(uint32_t))) {
                throw DebuggerError("Invalid or protected address");
            }
            
            if (!config.enable_writes) {
                throw DebuggerError("Memory writes are disabled");
            }
            
            logOperation("WRITE", address, value);
            
#ifdef _WIN32
            // Windows implementation
            HANDLE process = GetCurrentProcess();
            SIZE_T bytes_written;
            if (!WriteProcessMemory(process, (LPVOID)address, &value, sizeof(value), &bytes_written) ||
                bytes_written != sizeof(value)) {
                throw DebuggerError("WriteProcessMemory failed: " + 
                                    std::to_string(GetLastError()));
            }
#elif defined(__linux__)
            // Linux implementation using process_vm_writev
            struct iovec local_iov, remote_iov;
            local_iov.iov_base = &value;
            local_iov.iov_len = sizeof(value);
            remote_iov.iov_base = (void*)address;
            remote_iov.iov_len = sizeof(value);
            
            ssize_t bytes_written = process_vm_writev(getpid(), &local_iov, 1, 
                                                       &remote_iov, 1, 0);
            if (bytes_written != sizeof(value)) {
                throw DebuggerError("process_vm_writev failed");
            }
#elif defined(__APPLE__)
            // macOS implementation using mach_vm
            mach_port_t task = mach_task_self();
            vm_offset_t data = (vm_offset_t)&value;
            mach_msg_type_number_t data_size = sizeof(value);
            
            kern_return_t kr = mach_vm_write(task, (mach_vm_address_t)address, 
                                             data, data_size);
            if (kr != KERN_SUCCESS) {
                throw DebuggerError("mach_vm_write failed: " + std::to_string(kr));
            }
#else
            // Fallback: direct memory write (DANGEROUS, for demo only)
            volatile uint32_t* target = reinterpret_cast<volatile uint32_t*>(address);
            *target = value;
#endif
            
            // Verify the write by reading back
            result.read_back = readMemory(address);
            result.verification_passed = (result.read_back == value);
            
            if (!result.verification_passed) {
                logOperation("VERIFY_FAILED", address, result.read_back);
            }
            
            result.success = true;
            
        } catch (const std::exception& e) {
            result.error_message = e.what();
            logOperation("ERROR", address, 0);
        }
        
        return result;
    }
    
    /**
     * Read memory from address
     */
    uint32_t readMemory(uintptr_t address) {
        if (!validateAddress(address, sizeof(uint32_t))) {
            throw DebuggerError("Invalid address for reading");
        }
        
        if (!config.enable_reads) {
            throw DebuggerError("Memory reads are disabled");
        }
        
        uint32_t value = 0;
        
#ifdef _WIN32
        HANDLE process = GetCurrentProcess();
        SIZE_T bytes_read;
        if (!ReadProcessMemory(process, (LPVOID)address, &value, sizeof(value), &bytes_read) ||
            bytes_read != sizeof(value)) {
            throw DebuggerError("ReadProcessMemory failed");
        }
#elif defined(__linux__)
        struct iovec local_iov, remote_iov;
        local_iov.iov_base = &value;
        local_iov.iov_len = sizeof(value);
        remote_iov.iov_base = (void*)address;
        remote_iov.iov_len = sizeof(value);
        
        ssize_t bytes_read = process_vm_readv(getpid(), &local_iov, 1, 
                                               &remote_iov, 1, 0);
        if (bytes_read != sizeof(value)) {
            throw DebuggerError("process_vm_readv failed");
        }
#elif defined(__APPLE__)
        mach_port_t task = mach_task_self();
        vm_offset_t data;
        mach_msg_type_number_t data_size;
        
        kern_return_t kr = mach_vm_read(task, (mach_vm_address_t)address, 
                                        sizeof(value), &data, &data_size);
        if (kr != KERN_SUCCESS) {
            throw DebuggerError("mach_vm_read failed");
        }
        
        memcpy(&value, (void*)data, sizeof(value));
        mach_vm_deallocate(task, data, data_size);
#else
        volatile uint32_t* source = reinterpret_cast<volatile uint32_t*>(address);
        value = *source;
#endif
        
        logOperation("READ", address, value);
        return value;
    }
    
    /**
     * Interactive memory modification
     */
    void interactiveDebugger() {
        std::cout << "\n=== Memory Debugger Interactive Mode ===\n";
        std::cout << "Commands:\n";
        std::cout << "  w <address> <hexvalue> - Write 32-bit hex value to address\n";
        std::cout << "  r <address>            - Read from address\n";
        std::cout << "  watch <address>        - Add watch point\n";
        std::cout << "  list                    - List watch points\n";
        std::cout << "  config                  - Show configuration\n";
        std::cout << "  quit                    - Exit\n\n";
        
        std::string line;
        while (true) {
            std::cout << "debug> ";
            std::getline(std::cin, line);
            
            if (line == "quit") break;
            
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;
            
            if (cmd == "w" || cmd == "write") {
                std::string addr_str, val_str;
                iss >> addr_str >> val_str;
                
                try {
                    uintptr_t addr = std::stoull(addr_str, nullptr, 16);
                    uint32_t value = std::stoul(val_str, nullptr, 16);
                    
                    auto result = writeMemory(addr, value);
                    
                    if (result.success) {
                        std::cout << "✓ Wrote 0x" << std::hex << value 
                                  << " to 0x" << addr << std::dec << "\n";
                        if (result.verification_passed) {
                            std::cout << "  Verification: PASSED (read back: 0x" 
                                      << std::hex << result.read_back << std::dec << ")\n";
                        } else {
                            std::cout << "  Verification: FAILED (read back: 0x" 
                                      << std::hex << result.read_back << std::dec << ")\n";
                        }
                    } else {
                        std::cout << "✗ Error: " << result.error_message << "\n";
                    }
                } catch (const std::exception& e) {
                    std::cout << "✗ Invalid input: " << e.what() << "\n";
                }
            }
            else if (cmd == "r" || cmd == "read") {
                std::string addr_str;
                iss >> addr_str;
                
                try {
                    uintptr_t addr = std::stoull(addr_str, nullptr, 16);
                    uint32_t value = readMemory(addr);
                    std::cout << "0x" << std::hex << addr << ": 0x" 
                              << value << std::dec << "\n";
                } catch (const std::exception& e) {
                    std::cout << "✗ Error: " << e.what() << "\n";
                }
            }
            else if (cmd == "watch") {
                std::string addr_str;
                iss >> addr_str;
                
                try {
                    uintptr_t addr = std::stoull(addr_str, nullptr, 16);
                    watch_points[addr] = "Watch point";
                    std::cout << "Added watch point at 0x" << std::hex << addr << std::dec << "\n";
                } catch (const std::exception& e) {
                    std::cout << "✗ Invalid address\n";
                }
            }
            else if (cmd == "list") {
                if (watch_points.empty()) {
                    std::cout << "No watch points\n";
                } else {
                    std::cout << "Watch points:\n";
                    for (const auto& [addr, desc] : watch_points) {
                        std::cout << "  0x" << std::hex << addr << std::dec << ": " << desc << "\n";
                    }
                }
            }
            else if (cmd == "config") {
                std::cout << "\nCurrent Configuration:\n";
                std::cout << "  Writes enabled: " << (config.enable_writes ? "Yes" : "No") << "\n";
                std::cout << "  Reads enabled: " << (config.enable_reads ? "Yes" : "No") << "\n";
                std::cout << "  Validation: " << (config.require_validation ? "On" : "Off") << "\n";
                std::cout << "  Logging: " << (config.log_operations ? "On" : "Off") << "\n";
                std::cout << "  Valid range: 0x" << std::hex << config.min_valid_address 
                          << " - 0x" << config.max_valid_address << std::dec << "\n";
            }
            else if (!cmd.empty()) {
                std::cout << "Unknown command. Available: w, r, watch, list, config, quit\n";
            }
        }
    }
};

/**
 * Memory patching utility (for demonstration)
 */
void demonstrateMemoryPatching() {
    std::cout << "\n=== Memory Patching Demonstration ===\n";
    
    // Create a test variable
    volatile uint32_t test_variable = 0x12345678;
    uintptr_t test_addr = reinterpret_cast<uintptr_t>(
        const_cast<uint32_t*>(&test_variable));
    
    std::cout << "Test variable at address: 0x" << std::hex << test_addr << std::dec << "\n";
    std::cout << "Initial value: 0x" << std::hex << test_variable << std::dec << "\n";
    
    DebuggerConfig config;
    config.require_validation = false;  // Allow any address for demo
    MemoryDebugger debugger(config);
    
    // Write new value
    auto result = debugger.writeMemory(test_addr, 0xDEADBEEF);
    
    if (result.success) {
        std::cout << "After write: 0x" << std::hex << test_variable << std::dec << "\n";
        
        if (test_variable == 0xDEADBEEF) {
            std::cout << "✓ Memory successfully modified!\n";
        } else {
            std::cout << "✗ Memory modification failed\n";
        }
    }
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "Memory Debugger Utility\n";
    std::cout << "Write 32-bit hex values to memory addresses\n";
    std::cout << "========================================\n\n";
    
    try {
        // Parse command line
        if (argc == 3) {
            // Command line mode: ./debugger <address> <value>
            uintptr_t addr = std::stoull(argv[1], nullptr, 16);
            uint32_t value = std::stoul(argv[2], nullptr, 16);
            
            DebuggerConfig config;
            config.require_validation = false;  // Command line overrides safety
            
            MemoryDebugger debugger(config);
            auto result = debugger.writeMemory(addr, value);
            
            if (result.success) {
                std::cout << "Successfully wrote 0x" << std::hex << value 
                          << " to 0x" << addr << std::dec << "\n";
                return 0;
            } else {
                std::cerr << "Failed: " << result.error_message << "\n";
                return 1;
            }
        } else {
            // Interactive mode
            demonstrateMemoryPatching();
            
            std::cout << "\nStarting interactive debugger...\n";
            DebuggerConfig config;
            MemoryDebugger debugger(config);
            debugger.interactiveDebugger();
        }
        
        std::cout << "\n=== Safety Notes ===\n";
        std::cout << "⚠️  Writing to arbitrary memory addresses can:\n";
        std::cout << "   - Crash the program\n";
        std::cout << "   - Corrupt data\n";
        std::cout << "   - Create security vulnerabilities\n";
        std::cout << "   - Have undefined behavior\n\n";
        std::cout << "This tool should only be used for debugging on your own programs!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}