#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <chrono>
#include <thread>

/**
 * Custom exception for menu errors
 */
class MenuError : public std::runtime_error {
public:
    explicit MenuError(const std::string& message) 
        : std::runtime_error("Menu Error: " + message) {}
};

/**
 * Menu system class demonstrating switch statement
 */
class MenuSystem {
private:
    std::string menu_title;
    bool running;
    int selected_option;
    std::vector<std::string> menu_items;
    
    // Statistics for demonstration
    int option1_count;
    int option2_count;
    int other_count;
    
public:
    MenuSystem(const std::string& title) 
        : menu_title(title)
        , running(false)
        , selected_option(0)
        , option1_count(0)
        , option2_count(0)
        , other_count(0) {
        
        // Default menu items
        menu_items = {
            "Option 1 - Process Data",
            "Option 2 - Generate Report",
            "Option 3 - Settings",
            "Option 4 - Help",
            "Option 5 - About",
            "Exit"
        };
    }
    
    void displayMenu() const {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "   " << menu_title << "\n";
        std::cout << std::string(50, '=') << "\n";
        
        for (size_t i = 0; i < menu_items.size(); ++i) {
            std::cout << "   " << (i + 1) << ". " << menu_items[i] << "\n";
        }
        std::cout << std::string(50, '-') << "\n";
        std::cout << "Enter your choice (1-" << menu_items.size() << "): ";
    }
    
    int getUserChoice() {
        int choice;
        std::cin >> choice;
        
        // Clear input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        return choice;
    }
    
    /**
     * SIMPLE SWITCH - Case 1 and 2 execute their own code
     */
    void processChoiceSimple(int choice) {
        std::cout << "\n--- Processing choice " << choice << " ---\n";
        
        switch (choice) {
            case 1:
                // Case 1 own code
                std::cout << "  Case 1: Processing data...\n";
                option1_count++;
                // Simulate work
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << "  Data processing complete!\n";
                break;
                
            case 2:
                // Case 2 own code
                std::cout << "  Case 2: Generating report...\n";
                option2_count++;
                // Simulate work
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                std::cout << "  Report generated successfully!\n";
                break;
                
            case 3:
                std::cout << "  Settings menu (not implemented in this demo)\n";
                other_count++;
                break;
                
            case 4:
                std::cout << "  Help: This is a demonstration menu system\n";
                other_count++;
                break;
                
            case 5:
                std::cout << "  About: Menu System v1.0\n";
                other_count++;
                break;
                
            case 6:
                std::cout << "  Exiting menu system...\n";
                running = false;
                break;
                
            default:
                std::cout << "  Invalid option! Please choose 1-6.\n";
                other_count++;
        }
    }
    
    /**
     * Enhanced switch with more features
     */
    void processChoiceEnhanced(int choice) {
        std::cout << "\n" << std::string(40, '-') << "\n";
        std::cout << "Enhanced processing for option " << choice << ":\n";
        
        switch (choice) {
            case 1: {
                // Case 1 with its own scope
                std::cout << "  ⚙️  Executing Option 1: Data Processing\n";
                std::cout << "  Initializing processors...\n";
                
                // Local variables specific to case 1
                int data_size = 100;
                std::string data_type = "customer";
                
                std::cout << "  Processing " << data_size << " " << data_type << " records\n";
                
                // Simulate different stages
                for (int i = 1; i <= 3; ++i) {
                    std::cout << "  Stage " << i << "...\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                
                std::cout << "  ✅ Data processing completed\n";
                option1_count++;
                break;
            }
            
            case 2: {
                // Case 2 with its own scope
                std::cout << "  📊 Executing Option 2: Report Generation\n";
                
                // Local variables specific to case 2
                std::string report_type;
                std::cout << "  Enter report type (summary/detailed): ";
                std::getline(std::cin, report_type);
                
                if (report_type == "summary") {
                    std::cout << "  Generating summary report...\n";
                } else if (report_type == "detailed") {
                    std::cout << "  Generating detailed report...\n";
                } else {
                    std::cout << "  Using default report format\n";
                }
                
                // Generate report
                std::cout << "  Report sections:\n";
                std::cout << "    - Header\n";
                std::cout << "    - Data analysis\n";
                std::cout << "    - Statistics\n";
                std::cout << "    - Footer\n";
                
                std::cout << "  ✅ Report generated successfully\n";
                option2_count++;
                break;
            }
            
            case 3:
                std::cout << "  ⚙️  Settings\n";
                std::cout << "  This would open settings configuration\n";
                other_count++;
                break;
                
            case 4:
                std::cout << "  ❓ Help\n";
                std::cout << "  Available options:\n";
                std::cout << "    1: Process data\n";
                std::cout << "    2: Generate reports\n";
                std::cout << "    3: Configure settings\n";
                std::cout << "    4: Show this help\n";
                std::cout << "    5: About this program\n";
                std::cout << "    6: Exit\n";
                other_count++;
                break;
                
            case 5:
                std::cout << "  ℹ️  About\n";
                std::cout << "  Menu System Demo v2.0\n";
                std::cout << "  A comprehensive example of switch statements\n";
                other_count++;
                break;
                
            case 6:
                std::cout << "  👋 Exiting...\n";
                running = false;
                break;
                
            default:
                std::cout << "  ❌ Invalid choice! Please enter 1-6.\n";
                other_count++;
        }
    }
    
    /**
     * Run the menu system
     */
    void run() {
        running = true;
        
        while (running) {
            displayMenu();
            int choice = getUserChoice();
            processChoiceEnhanced(choice);
        }
        
        printStatistics();
    }
    
    /**
     * Print usage statistics
     */
    void printStatistics() const {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "Session Statistics\n";
        std::cout << std::string(50, '=') << "\n";
        std::cout << "Option 1 (Data Processing) used: " << option1_count << " times\n";
        std::cout << "Option 2 (Report Generation) used: " << option2_count << " times\n";
        std::cout << "Other options used: " << other_count << " times\n";
        std::cout << "Total selections: " << (option1_count + option2_count + other_count) << "\n";
    }
};

/**
 * Demonstrates different switch statement patterns
 */
void demonstrateSwitchPatterns() {
    std::cout << "\n=== Switch Statement Patterns ===\n";
    
    // Pattern 1: Basic switch with breaks
    std::cout << "\nPattern 1: Basic switch with breaks:\n";
    for (int test : {1, 2, 3}) {
        std::cout << "Testing value " << test << ": ";
        switch (test) {
            case 1:
                std::cout << "Case 1 executed";
                break;
            case 2:
                std::cout << "Case 2 executed";
                break;
            default:
                std::cout << "Default case";
        }
        std::cout << "\n";
    }
    
    // Pattern 2: Switch with fallthrough (rare, use [[fallthrough]])
    std::cout << "\nPattern 2: Switch with intentional fallthrough:\n";
    int level = 1;
    switch (level) {
        case 1:
            std::cout << "Level 1 processing\n";
            [[fallthrough]];
        case 2:
            std::cout << "Level 2 processing\n";
            break;
        default:
            std::cout << "Default processing\n";
    }
    
    // Pattern 3: Multiple cases same code
    std::cout << "\nPattern 3: Multiple cases same code:\n";
    char grade = 'B';
    switch (grade) {
        case 'A':
        case 'B':
        case 'C':
            std::cout << "Passing grade\n";
            break;
        case 'D':
        case 'F':
            std::cout << "Failing grade\n";
            break;
        default:
            std::cout << "Invalid grade\n";
    }
}

/**
 * Simple menu function - the core request
 */
void simpleMenu() {
    std::cout << "\n=== SIMPLE MENU (Core Request) ===\n";
    std::cout << "1. Option 1\n";
    std::cout << "2. Option 2\n";
    std::cout << "3. Exit\n";
    std::cout << "Choice: ";
    
    int choice;
    std::cin >> choice;
    
    // The requested switch statement
    switch (choice) {
        case 1:
            std::cout << "Executing Option 1 code\n";
            // Option 1 specific code here
            break;
            
        case 2:
            std::cout << "Executing Option 2 code\n";
            // Option 2 specific code here
            break;
            
        case 3:
            std::cout << "Exiting...\n";
            break;
            
        default:
            std::cout << "Invalid choice!\n";
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Menu System with Switch Statement\n";
    std::cout << "Case 1 and 2 Execute Own Code\n";
    std::cout << "========================================\n";
    
    // Simple demonstration
    simpleMenu();
    
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Demonstrate switch patterns
    demonstrateSwitchPatterns();
    
    // Run full menu system
    std::cout << "\nPress Enter to start the full menu system...\n";
    std::cin.get();
    
    MenuSystem menu("Main Menu System");
    menu.run();
    
    // Summary
    std::cout << "\n=== Switch Statement Summary ===\n";
    std::cout << "✓ Case 1 executes its own code\n";
    std::cout << "✓ Case 2 executes its own code\n";
    std::cout << "✓ Each case has its own break\n";
    std::cout << "✓ Default case handles invalid input\n";
    std::cout << "✓ Cases can have their own scope with {}\n";
    std::cout << "✓ Multiple cases can share code\n";
    std::cout << "✓ Fallthrough can be intentional (use [[fallthrough]])\n";
    
    return 0;
}