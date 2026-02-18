#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <system_error>
#include <thread>
#include <chrono>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
#endif

/**
 * Custom exception for memory access errors
 */
class MemoryAccessError : public std::runtime_error {
public:
    explicit MemoryAccessError(const std::string& message) 
        : std::runtime_error("Memory Access Error: " + message) {}
};

/**
 * Configuration for memory access
 */
struct MemoryAccessConfig {
    uintptr_t address;
    size_t num_flags;
    bool use_volatile;
    bool simulate_hardware;
    int timeout_ms;
    int retry_count;
    
    MemoryAccessConfig() 
        : address(0x00004500)
        , num_flags(4)
        , use_volatile(true)
        , simulate_hardware(true)
        , timeout_ms(1000)
        , retry_count(3) {}
};

/**
 * Status flag register representation
 */
struct StatusRegister {
    union {
        struct {
            uint8_t ready : 1;      // Bit 0: Device ready
            uint8_t busy : 1;        // Bit 1: Device busy
            uint8_t error : 1;        // Bit 2: Error occurred
            uint8_t data_available : 1; // Bit 3: Data available
            uint8_t overflow : 1;     // Bit 4: Buffer overflow
            uint8_t underflow : 1;    // Bit 5: Buffer underflow
            uint8_t timeout : 1;      // Bit 6: Operation timeout
            uint8_t reserved : 1;     // Bit 7: Reserved
        } flags;
        uint8_t raw;
    };
    
    StatusRegister() : raw(0) {}
    
    void print() const {
        std::cout << "Status Register (0x" << std::hex << std::setw(2) 
                  << std::setfill('0') << static_cast<int>(raw) << std::dec << "):\n";
        std::cout << "  Ready:          " << (flags.ready ? "✓" : "✗") << "\n";
        std::cout << "  Busy:           " << (flags.busy ? "✓" : "✗") << "\n";
        std::cout << "  Error:          " << (flags.error ? "✓" : "✗") << "\n";
        std::cout << "  Data Available: " << (flags.data_available ? "✓" : "✗") << "\n";
        std::cout << "  Overflow:       " << (flags.overflow ? "✓" : "✗") << "\n";
        std::cout << "  Underflow:      " << (flags.underflow ? "✓" : "✗") << "\n";
        std::cout << "  Timeout:        " << (flags.timeout ? "✓" : "✗") << "\n";
    }
};

/**
 * Hardware status register array
 */
struct HardwareStatus {
    StatusRegister status[4];  // 4 status registers at sequential addresses
    
    void printAll() const {
        for (int i = 0; i < 4; ++i) {
            std::cout << "\nStatus Register " << i << ":\n";
            status[i].print();
        }
    }
};

/**
 * Base class for memory access
 */
class MemoryAccessor {
protected:
    uintptr_t base_address;
    bool is_mapped;
    
public:
    explicit MemoryAccessor(uintptr_t addr) : base_address(addr), is_mapped(false) {}
    virtual ~MemoryAccessor() = default;
    
    virtual bool mapMemory() = 0;
    virtual void unmapMemory() = 0;
    virtual uint8_t readByte(size_t offset) = 0;
    virtual void writeByte(size_t offset, uint8_t value) = 0;
    
    bool isMapped() const { return is_mapped; }
};

/**
 * SIMULATION: Memory access for demonstration (safe, no real hardware access)
 */
class SimulatedMemoryAccessor : public MemoryAccessor {
private:
    HardwareStatus simulated_hardware;
    
public:
    explicit SimulatedMemoryAccessor(uintptr_t addr) 
        : MemoryAccessor(addr) {
        
        std::cout << "[SIM] Initializing simulated hardware at 0x" 
                  << std::hex << addr << std::dec << "\n";
        
        // Initialize with some test values
        simulated_hardware.status[0].raw = 0b00000101;  // Ready + Data Available
        simulated_hardware.status[1].raw = 0b00000010;  // Busy
        simulated_hardware.status[2].raw = 0b00000100;  // Error
        simulated_hardware.status[3].raw = 0b00110000;  // Overflow + Underflow
        
        is_mapped = true;
    }
    
    bool mapMemory() override { return true; }
    void unmapMemory() override {}
    
    uint8_t readByte(size_t offset) override {
        if (offset >= sizeof(HardwareStatus)) {
            throw MemoryAccessError("Offset out of range");
        }
        
        // Simulate hardware updating flags
        if (offset < 4) {  // Status registers
            // Simulate changing flags
            if (simulated_hardware.status[offset].flags.busy) {
                // Simulate busy becoming ready after some reads
                static int read_count[4] = {0};
                if (++read_count[offset] > 3) {
                    simulated_hardware.status[offset].flags.busy = 0;
                    simulated_hardware.status[offset].flags.ready = 1;
                    simulated_hardware.status[offset].flags.data_available = 1;
                }
            }
        }
        
        return reinterpret_cast<uint8_t*>(&simulated_hardware)[offset];
    }
    
    void writeByte(size_t offset, uint8_t value) override {
        if (offset >= sizeof(HardwareStatus)) {
            throw MemoryAccessError("Offset out of range");
        }
        
        // Simulate writing to hardware
        reinterpret_cast<uint8_t*>(&simulated_hardware)[offset] = value;
        std::cout << "[SIM] Wrote 0x" << std::hex << static_cast<int>(value) 
                  << std::dec << " to offset " << offset << "\n";
    }
};

#ifdef __linux__
/**
 * Linux actual hardware access (requires root)
 */
class LinuxMemoryAccessor : public MemoryAccessor {
private:
    int mem_fd;
    void* mapped_addr;
    size_t mapping_size;
    
public:
    LinuxMemoryAccessor(uintptr_t addr, size_t size = 4096) 
        : MemoryAccessor(addr), mem_fd(-1), mapped_addr(nullptr), mapping_size(size) {}
    
    ~LinuxMemoryAccessor() {
        unmapMemory();
    }
    
    bool mapMemory() override {
        // Open /dev/mem (requires root)
        mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (mem_fd == -1) {
            std::cerr << "Failed to open /dev/mem: " << strerror(errno) << "\n";
            std::cerr << "This operation typically requires root privileges\n";
            return false;
        }
        
        // Map physical memory
        mapped_addr = mmap(nullptr, mapping_size, PROT_READ | PROT_WRITE, 
                          MAP_SHARED, mem_fd, base_address);
        
        if (mapped_addr == MAP_FAILED) {
            std::cerr << "Failed to map memory: " << strerror(errno) << "\n";
            close(mem_fd);
            mem_fd = -1;
            return false;
        }
        
        is_mapped = true;
        std::cout << "[Linux] Mapped " << mapping_size << " bytes at 0x" 
                  << std::hex << base_address << " -> " << mapped_addr << std::dec << "\n";
        return true;
    }
    
    void unmapMemory() override {
        if (mapped_addr && mapped_addr != MAP_FAILED) {
            munmap(mapped_addr, mapping_size);
            mapped_addr = nullptr;
        }
        if (mem_fd != -1) {
            close(mem_fd);
            mem_fd = -1;
        }
        is_mapped = false;
    }
    
    uint8_t readByte(size_t offset) override {
        if (!is_mapped || !mapped_addr) {
            throw MemoryAccessError("Memory not mapped");
        }
        
        volatile uint8_t* ptr = static_cast<volatile uint8_t*>(mapped_addr) + offset;
        return *ptr;
    }
    
    void writeByte(size_t offset, uint8_t value) override {
        if (!is_mapped || !mapped_addr) {
            throw MemoryAccessError("Memory not mapped");
        }
        
        volatile uint8_t* ptr = static_cast<volatile uint8_t*>(mapped_addr) + offset;
        *ptr = value;
    }
};
#endif

#ifdef _WIN32
/**
 * Windows actual hardware access (requires driver)
 */
class WindowsMemoryAccessor : public MemoryAccessor {
private:
    HANDLE physical_handle;
    void* mapped_addr;
    size_t mapping_size;
    
public:
    WindowsMemoryAccessor(uintptr_t addr, size_t size = 4096) 
        : MemoryAccessor(addr), physical_handle(nullptr), mapped_addr(nullptr), mapping_size(size) {}
    
    ~WindowsMemoryAccessor() {
        unmapMemory();
    }
    
    bool mapMemory() override {
        // This requires a kernel driver on Windows
        // For demonstration, we'll use VirtualAlloc for simulated access
        std::cout << "[Windows] Direct hardware access requires kernel driver\n";
        std::cout << "[Windows] Using simulated memory for demonstration\n";
        
        mapped_addr = VirtualAlloc(nullptr, mapping_size, MEM_COMMIT, PAGE_READWRITE);
        if (!mapped_addr) {
            std::cerr << "Failed to allocate memory: " << GetLastError() << "\n";
            return false;
        }
        
        is_mapped = true;
        return true;
    }
    
    void unmapMemory() override {
        if (mapped_addr) {
            VirtualFree(mapped_addr, 0, MEM_RELEASE);
            mapped_addr = nullptr;
        }
        is_mapped = false;
    }
    
    uint8_t readByte(size_t offset) override {
        if (!is_mapped || !mapped_addr) {
            throw MemoryAccessError("Memory not mapped");
        }
        
        volatile uint8_t* ptr = static_cast<volatile uint8_t*>(mapped_addr) + offset;
        return *ptr;
    }
    
    void writeByte(size_t offset, uint8_t value) override {
        if (!is_mapped || !mapped_addr) {
            throw MemoryAccessError("Memory not mapped");
        }
        
        volatile uint8_t* ptr = static_cast<volatile uint8_t*>(mapped_addr) + offset;
        *ptr = value;
    }
};
#endif

/**
 * Driver class for reading status flags
 */
class StatusFlagDriver {
private:
    std::unique_ptr<MemoryAccessor> accessor;
    MemoryAccessConfig config;
    
public:
    explicit StatusFlagDriver(const MemoryAccessConfig& cfg) 
        : config(cfg) {
        
        std::cout << "Initializing StatusFlagDriver for address 0x" 
                  << std::hex << cfg.address << std::dec << "\n";
        
        // Create appropriate accessor based on platform
        if (config.simulate_hardware) {
            accessor = std::make_unique<SimulatedMemoryAccessor>(cfg.address);
        } else {
#ifdef __linux__
            accessor = std::make_unique<LinuxMemoryAccessor>(cfg.address);
#elif _WIN32
            accessor = std::make_unique<WindowsMemoryAccessor>(cfg.address);
#else
            throw MemoryAccessError("No hardware access implementation for this platform");
#endif
        }
        
        if (!accessor->mapMemory()) {
            throw MemoryAccessError("Failed to map memory");
        }
    }
    
    ~StatusFlagDriver() {
        if (accessor) {
            accessor->unmapMemory();
        }
    }
    
    /**
     * Read a single status register
     */
    StatusRegister readStatusRegister(int index) {
        if (index < 0 || index >= config.num_flags) {
            throw std::out_of_range("Invalid status register index");
        }
        
        size_t offset = index * sizeof(StatusRegister);
        uint8_t raw_value = accessor->readByte(offset);
        
        StatusRegister reg;
        reg.raw = raw_value;
        
        return reg;
    }
    
    /**
     * Read all status registers
     */
    HardwareStatus readAllStatus() {
        HardwareStatus status;
        
        for (size_t i = 0; i < config.num_flags; ++i) {
            size_t offset = i * sizeof(StatusRegister);
            uint8_t raw_value = accessor->readByte(offset);
            status.status[i].raw = raw_value;
        }
        
        return status;
    }
    
    /**
     * Wait for a specific flag to be set
     */
    bool waitForFlag(int index, uint8_t flag_mask, bool value) {
        auto start = std::chrono::steady_clock::now();
        
        for (int attempt = 0; attempt < config.retry_count; ++attempt) {
            uint8_t reg = accessor->readByte(index);
            bool flag_state = (reg & flag_mask) != 0;
            
            if (flag_state == value) {
                return true;
            }
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - start);
            
            if (elapsed.count() > config.timeout_ms) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        return false;
    }
    
    /**
     * Clear a flag
     */
    void clearFlag(int index, uint8_t flag_mask) {
        uint8_t current = accessor->readByte(index);
        uint8_t new_value = current & ~flag_mask;
        accessor->writeByte(index, new_value);
    }
    
    /**
     * Set a flag
     */
    void setFlag(int index, uint8_t flag_mask) {
        uint8_t current = accessor->readByte(index);
        uint8_t new_value = current | flag_mask;
        accessor->writeByte(index, new_value);
    }
};

/**
 * Simple driver function - reads status flags from hardcoded address
 */
void readStatusFromHardcodedAddress() {
    std::cout << "\n=== Simple Status Read from 0x00004500 ===\n";
    
    // Hardcoded address - exactly as requested
    const uintptr_t STATUS_ADDRESS = 0x00004500;
    
    // In real hardware, this would be a volatile pointer
    volatile uint32_t* status_register = 
        reinterpret_cast<volatile uint32_t*>(STATUS_ADDRESS);
    
    std::cout << "Attempting to read from address: 0x" 
              << std::hex << STATUS_ADDRESS << std::dec << "\n";
    
    // DANGER: This would crash on most systems without proper mapping
    std::cout << "WARNING: Direct dereference of 0x" << std::hex << STATUS_ADDRESS 
              << std::dec << " would likely crash!\n";
    std::cout << "Using simulation instead...\n";
    
    // Simulated read (safe for demonstration)
    SimulatedMemoryAccessor sim(STATUS_ADDRESS);
    StatusRegister reg;
    reg.raw = sim.readByte(0);
    
    std::cout << "\nSimulated status flags at 0x00004500:\n";
    reg.print();
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Memory-Mapped I/O: Reading Status Flags\n";
    std::cout << "========================================\n\n";
    
    try {
        // Simple hardcoded address read
        readStatusFromHardcodedAddress();
        
        // Configuration for more comprehensive example
        MemoryAccessConfig config;
        config.address = 0x00004500;
        config.num_flags = 4;
        config.simulate_hardware = true;
        
        // Create driver
        StatusFlagDriver driver(config);
        
        // Read all status flags
        std::cout << "\n=== Reading All Status Registers ===\n";
        auto all_status = driver.readAllStatus();
        all_status.printAll();
        
        // Read individual registers
        std::cout << "\n=== Reading Individual Status Registers ===\n";
        for (int i = 0; i < 4; ++i) {
            StatusRegister reg = driver.readStatusRegister(i);
            std::cout << "\nRegister " << i << ":\n";
            reg.print();
        }
        
        // Monitor for changes
        std::cout << "\n=== Monitoring Status Changes ===\n";
        std::cout << "Watching for status changes (press Ctrl+C to stop)...\n";
        
        for (int i = 0; i < 10; ++i) {
            auto status = driver.readAllStatus();
            
            std::cout << "\n--- Status at iteration " << i << " ---\n";
            for (int j = 0; j < 4; ++j) {
                std::cout << "Reg " << j << ": 0x" << std::hex 
                          << std::setw(2) << std::setfill('0')
                          << static_cast<int>(status.status[j].raw) << " ";
            }
            std::cout << std::dec << "\n";
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        // Wait for specific flag
        std::cout << "\n=== Waiting for Ready Flag ===\n";
        if (driver.waitForFlag(0, 0x01, true)) {
            std::cout << "Ready flag detected!\n";
        } else {
            std::cout << "Timeout waiting for ready flag\n";
        }
        
        // Manipulate flags
        std::cout << "\n=== Manipulating Status Flags ===\n";
        std::cout << "Setting error flag...\n";
        driver.setFlag(2, 0x04);  // Set error flag on register 2
        
        StatusRegister reg2 = driver.readStatusRegister(2);
        reg2.print();
        
        std::cout << "\nClearing error flag...\n";
        driver.clearFlag(2, 0x04);
        
        reg2 = driver.readStatusRegister(2);
        reg2.print();
        
        // Platform-specific information
        std::cout << "\n=== Platform Information ===\n";
#ifdef __linux__
        std::cout << "Platform: Linux\n";
        std::cout << "For real hardware access:\n";
        std::cout << "  - Compile with: g++ -std=c++17 -o driver driver.cpp\n";
        std::cout << "  - Run with: sudo ./driver\n";
        std::cout << "  - Requires /dev/mem access\n";
#elif _WIN32
        std::cout << "Platform: Windows\n";
        std::cout << "For real hardware access:\n";
        std::cout << "  - Requires kernel driver\n";
        std::cout << "  - Use DeviceIoControl for driver communication\n";
#elif __APPLE__
        std::cout << "Platform: macOS\n";
        std::cout << "Direct hardware access is restricted on macOS\n";
#else
        std::cout << "Platform: Unknown\n";
#endif
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "✓ Status flags read from 0x00004500 (simulated)\n";
        std::cout << "✓ Individual flag bits accessed\n";
        std::cout << "✓ Flag monitoring implemented\n";
        std::cout << "✓ Flag manipulation demonstrated\n";
        std::cout << "✓ Platform-specific considerations noted\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}