#include <iostream>
#include <iomanip>
#include <limits>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>

// Safe input methods for char arrays

// Method 1: Using cin.getline() - Recommended for most cases
void readWithCinGetline() {
    std::cout << "\n=== Method 1: cin.getline() ===\n";
    
    const int BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE];
    
    std::cout << "Enter text (max " << BUFFER_SIZE - 1 << " chars): ";
    std::cin.getline(buffer, BUFFER_SIZE);
    
    std::cout << "You entered: \"" << buffer << "\"\n";
    std::cout << "Length: " << std::strlen(buffer) << " characters\n";
}

// Method 2: Using cin.get() with delimiter
void readWithCinGet() {
    std::cout << "\n=== Method 2: cin.get() with delimiter ===\n";
    
    const int BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE];
    
    std::cout << "Enter text (will stop at newline, max " << BUFFER_SIZE - 1 << " chars): ";
    std::cin.get(buffer, BUFFER_SIZE);
    
    // Clear the newline left in the stream
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "You entered: \"" << buffer << "\"\n";
}

// Method 3: Character by character reading with overflow protection
void readCharByChar() {
    std::cout << "\n=== Method 3: Character-by-character reading ===\n";
    
    const int BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE];
    int index = 0;
    char ch;
    
    std::cout << "Enter text (press Enter to finish): ";
    
    while (index < BUFFER_SIZE - 1) {  // Leave room for null terminator
        ch = std::cin.get();
        
        if (ch == '\n') {
            break;
        }
        
        buffer[index++] = ch;
    }
    
    buffer[index] = '\0';  // Null terminate
    
    // Clear any remaining characters in input buffer
    if (!std::cin.eof() && ch != '\n') {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    std::cout << "You entered: \"" << buffer << "\"\n";
    std::cout << "Length: " << index << " characters\n";
}

// Method 4: Using std::string with conversion to char array
void readWithString() {
    std::cout << "\n=== Method 4: std::string to char array ===\n";
    
    const int BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE] = {0};  // Initialize to all zeros
    std::string input;
    
    std::cout << "Enter text: ";
    std::getline(std::cin, input);
    
    // Safe copy to char array
    size_t copy_length = std::min(input.length(), static_cast<size_t>(BUFFER_SIZE - 1));
    std::copy(input.begin(), input.begin() + copy_length, buffer);
    buffer[copy_length] = '\0';
    
    std::cout << "You entered: \"" << buffer << "\"\n";
    std::cout << "Original string length: " << input.length() << "\n";
    std::cout << "Copied to buffer: " << std::strlen(buffer) << " chars\n";
    
    // Check for truncation
    if (input.length() >= BUFFER_SIZE) {
        std::cout << "⚠️  Input was truncated!\n";
    }
}

// Method 5: Using cin.getline() with custom delimiter
void readWithCustomDelimiter() {
    std::cout << "\n=== Method 5: Custom delimiter (',') ===\n";
    
    const int BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE];
    
    std::cout << "Enter text (will stop at comma or newline): ";
    std::cin.getline(buffer, BUFFER_SIZE, ',');
    
    // Clear the newline if present
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "You entered: \"" << buffer << "\"\n";
}

// Method 6: Secure input with explicit bounds checking
class SafeCharArrayReader {
private:
    static const int MAX_ATTEMPTS = 3;
    
public:
    struct ReadResult {
        bool success;
        char* buffer;
        size_t length;
        bool truncated;
        std::string error_message;
    };
    
    static ReadResult readSecure(char* buffer, size_t buffer_size, 
                                 const std::string& prompt = "Enter text: ") {
        ReadResult result;
        result.buffer = buffer;
        result.success = false;
        result.truncated = false;
        result.length = 0;
        
        if (buffer == nullptr || buffer_size == 0) {
            result.error_message = "Invalid buffer";
            return result;
        }
        
        // Clear buffer initially for security
        std::fill_n(buffer, buffer_size, 0);
        
        std::cout << prompt;
        
        // Read with bounds checking
        if (std::cin.getline(buffer, buffer_size)) {
            result.success = true;
            result.length = std::strlen(buffer);
            
            // Check if we read the maximum allowed (might be truncated)
            if (result.length == buffer_size - 1) {
                // Check if there's more input waiting
                if (std::cin.peek() != '\n' && !std::cin.eof()) {
                    result.truncated = true;
                    // Clear the rest of the line
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        } else {
            // Handle read failure
            if (std::cin.eof()) {
                result.error_message = "End of file reached";
            } else if (std::cin.fail()) {
                result.error_message = "Input too long for buffer";
                // Clear error state and ignore remaining input
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                result.error_message = "Unknown error";
            }
        }
        
        return result;
    }
    
    static bool readWithRetry(char* buffer, size_t buffer_size, 
                              const std::string& prompt = "Enter text: ") {
        for (int attempt = 1; attempt <= MAX_ATTEMPTS; ++attempt) {
            auto result = readSecure(buffer, buffer_size, prompt);
            
            if (result.success && !result.truncated) {
                return true;
            }
            
            if (result.truncated) {
                std::cout << "Input truncated. Maximum length is " 
                          << buffer_size - 1 << " characters.\n";
                return false;
            }
            
            if (attempt < MAX_ATTEMPTS) {
                std::cout << "Invalid input. Please try again (attempt " 
                          << attempt << "/" << MAX_ATTEMPTS << "): ";
            }
        }
        
        std::cout << "Maximum attempts exceeded.\n";
        return false;
    }
};

// Method 7: Reading specific data types into char array
void readFormattedInput() {
    std::cout << "\n=== Method 7: Formatted input ===\n";
    
    const int BUFFER_SIZE = 30;
    char name[BUFFER_SIZE];
    int age;
    char city[BUFFER_SIZE];
    
    std::cout << "Enter name, age, and city (separated by spaces): ";
    std::cin >> std::setw(BUFFER_SIZE) >> name
             >> age
             >> std::setw(BUFFER_SIZE) >> city;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Name: \"" << name << "\"\n";
    std::cout << "Age: " << age << "\n";
    std::cout << "City: \"" << city << "\"\n";
}

// Method 8: Reading until EOF
void readUntilEOF() {
    std::cout << "\n=== Method 8: Reading until EOF ===\n";
    std::cout << "Enter multiple lines (Ctrl+D or Ctrl+Z to end):\n";
    
    const int LINE_SIZE = 100;
    const int MAX_LINES = 10;
    char lines[MAX_LINES][LINE_SIZE];
    int line_count = 0;
    
    while (line_count < MAX_LINES && 
           std::cin.getline(lines[line_count], LINE_SIZE)) {
        line_count++;
    }
    
    std::cout << "\nYou entered " << line_count << " lines:\n";
    for (int i = 0; i < line_count; ++i) {
        std::cout << "  Line " << i + 1 << ": \"" << lines[i] << "\"\n";
    }
    
    // Clear EOF flag if needed
    if (std::cin.eof()) {
        std::cin.clear();
    }
}

// Utility function to display buffer contents (including hidden chars)
void displayBufferHex(const char* buffer, size_t size) {
    std::cout << "Buffer hex dump:\n";
    for (size_t i = 0; i < size; ++i) {
        if (i % 16 == 0) std::cout << "\n  " << std::setw(4) << i << ": ";
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(static_cast<unsigned char>(buffer[i])) << " ";
    }
    std::cout << std::dec << "\n";
}

int main() {
    std::cout << "Safe Char Array Input Methods\n";
    std::cout << "=============================\n";
    
    try {
        // Method 1: cin.getline()
        readWithCinGetline();
        
        // Method 2: cin.get()
        readWithCinGet();
        
        // Method 3: Character by character
        readCharByChar();
        
        // Method 4: Using std::string
        readWithString();
        
        // Method 5: Custom delimiter
        readWithCustomDelimiter();
        
        // Method 6: Secure reader class
        std::cout << "\n=== Method 6: Secure reader class ===\n";
        const int BUFFER_SIZE = 20;
        char secure_buffer[BUFFER_SIZE];
        
        if (SafeCharArrayReader::readWithRetry(secure_buffer, BUFFER_SIZE)) {
            std::cout << "Successfully read: \"" << secure_buffer << "\"\n";
            displayBufferHex(secure_buffer, BUFFER_SIZE);
        }
        
        // Method 7: Formatted input
        readFormattedInput();
        
        // Method 8: Reading until EOF
        // Commented out to avoid blocking in automated testing
        // readUntilEOF();
        
        // Demonstrate common pitfalls
        std::cout << "\n=== Common Pitfalls to Avoid ===\n";
        
        // PITFALL 1: Using cin >> without width specification
        std::cout << "\nPitfall 1: Unsafe cin >> (NO!):\n";
        char unsafe_buffer[10];
        std::cout << "This would be unsafe - don't do this:\n";
        std::cout << "  char buffer[10];\n";
        std::cout << "  cin >> buffer;  // No bounds checking!\n";
        
        // PITFALL 2: Using gets() (deprecated and dangerous)
        std::cout << "\nPitfall 2: Never use gets():\n";
        std::cout << "  gets(buffer);  // COMPLETELY UNSAFE - removed from C++\n";
        
        // PITFALL 3: Not checking for null terminator
        std::cout << "\nPitfall 3: Forgetting null terminator:\n";
        char bad_buffer[5] = {'H', 'e', 'l', 'l', 'o'};  // No null terminator!
        std::cout << "  char bad_buffer[5] = {'H','e','l','l','o'};  // No null!\n";
        std::cout << "  strlen(bad_buffer) would read beyond buffer!\n";
        
        // Demonstrate buffer clearing for security
        std::cout << "\n=== Secure Buffer Clearing ===\n";
        char sensitive_buffer[50];
        std::cout << "Enter sensitive data (will be cleared): ";
        std::cin.getline(sensitive_buffer, 50);
        
        std::cout << "Data: \"" << sensitive_buffer << "\"\n";
        
        // Securely clear sensitive data
        std::fill_n(sensitive_buffer, 50, 0);
        std::cout << "Buffer cleared for security\n";
        
        // Additional safety tips
        std::cout << "\n=== Safety Tips ===\n";
        std::cout << "✓ Always specify buffer size (setw or getline)\n";
        std::cout << "✓ Check for input truncation\n";
        std::cout << "✓ Clear input buffer after reading\n";
        std::cout << "✓ Initialize char arrays (char buffer[100] = {0})\n";
        std::cout << "✓ Consider using std::string for dynamic input\n";
        std::cout << "✓ Securely clear sensitive data\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}