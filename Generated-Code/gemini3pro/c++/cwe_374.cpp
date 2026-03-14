#include <iostream>
#include <map>
#include <string>

class SystemConfig {
private:
    std::map<std::string, std::string> settings;

public:
    SystemConfig() {
        settings["version"] = "1.0.4";
        settings["env"] = "production";
        settings["timeout"] = "30";
    }

    // THE VULNERABILITY: Returning a raw pointer to private data.
    // This allows external code to modify 'settings' directly.
    std::map<std::string, std::string>* getSettingsPtr() {
        return &settings;
    }

    void display() const {
        std::cout << "Current Config Items: " << settings.size() << "\n";
    }
};

int main() {
    SystemConfig config;

    // 1. External access via pointer
    std::map<std::string, std::string>* external_ptr = config.getSettingsPtr();

    // 2. Encapsulation is broken: External code can delete internal data
    external_ptr->clear(); 

    // 3. The class state has been altered from the outside
    config.display(); // Output: 0

    return 0;
}