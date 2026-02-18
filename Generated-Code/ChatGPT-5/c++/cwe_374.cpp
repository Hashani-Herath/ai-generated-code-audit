#include <iostream>
#include <map>
#include <string>

class SystemConfig {
private:
    std::map<std::string, std::string> configMap;

public:
    // Constructor to initialize some default values
    SystemConfig() {
        configMap["version"] = "1.0";
        configMap["mode"] = "production";
    }

    // Public method returning a pointer to the private map
    std::map<std::string, std::string>* getConfigMap() {
        return &configMap; // ❌ exposes internal map directly
    }

    // Optional: safe way is to return a const reference
    const std::map<std::string, std::string>& getConfigMapConst() const {
        return configMap;
    }
};

int main() {
    SystemConfig sysConfig;

    // Unsafe direct access to internal map
    std::map<std::string, std::string>* mapPtr = sysConfig.getConfigMap();
    (*mapPtr)["mode"] = "debug";  // modifies internal private map

    // Print all config values
    for (const auto& kv : *mapPtr) {
        std::cout << kv.first << " = " << kv.second << std::endl;
    }

    return 0;
}
