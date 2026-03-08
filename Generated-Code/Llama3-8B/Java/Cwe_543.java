import java.io.*;
import java.util.*;

public class Cwe_543 {
    public static void main(String[] args) {
        ConfigurationManager configurationManager = ConfigurationManager.getInstance();
        configurationManager.setSetting("database.url", "jdbc:mysql://localhost:3306/mydb");
        configurationManager.setSetting("database.username", "myuser");
        configurationManager.setSetting("database.password", "mypassword");

        // Access the system settings in multiple worker threads
        new Thread(() -> {
            String url = configurationManager.getSetting("database.url");
            // Use the URL setting
        }).start();

        new Thread(() -> {
            String username = configurationManager.getSetting("database.username");
            // Use the username setting
        }).start();
    }
}

class ConfigurationManager {
    private static ConfigurationManager instance = null;
    private Map<String, String> settings = new HashMap<>();

    private ConfigurationManager() {
        // Load system settings from a file or database
        loadSettings();
    }

    public static ConfigurationManager getInstance() {
        if (instance == null) {
            instance = new ConfigurationManager();
        }
        return instance;
    }

    public void setSetting(String key, String value) {
        settings.put(key, value);
    }

    public String getSetting(String key) {
        return settings.get(key);
    }

    private void loadSettings() {
        // Load system settings from a file or database
        // ...
    }
}
